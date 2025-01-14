#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>
#include <netdb.h>
#include <errno.h>
#include "shared.h"

#define PORT "9999"
#define BUFFER_SIZE 256
#define MAX_CLIENTS 10

struct Player{ //might delete
  int position;
  int lives;
};

struct Client {
  int socket;
  char name[BUFFER_SIZE];
};

struct Client lobby[MAX_CLIENTS];
int lobby_count = 0;

void broadcast_lobby(){
  char buffer[BUFFER_SIZE] = "Lobby: ";
  for(int x=0;x<lobby_count;x++){
    strcat(buffer,lobby[x].name);
    strcat(buffer," ");
  }
  strcat(buffer,"\n");
  for(int x =0;x<lobby_count;x++){
    send(lobby[x].socket,buffer,strlen(buffer),0);
  }
}

void add_to_lobby(int client_socket, const char *name){
  if(lobby_count<MAX_CLIENTS){
    lobby[lobby_count].socket = client_socket;
    strncpy(lobby[lobby_count].name,name,BUFFER_SIZE);
    lobby_count++;
    broadcast_lobby();
  }else{
    char msg[] = "Lobby is full, try again later\n";
    send(client_socket,msg,strlen(msg),0);
    close(client_socket);
  }
}

void remove_from_lobby(int client_socket){
  for(int x=0;x<lobby_count;x++){
    if(lobby[x].socket==client_socket){
      for(y=x;y<lobby_count-1;y++){
        lobby[y] = lobby[y+1];
      }
      lobby_count--;
      break;
    }
  }
  broadcast_lobby();
}

void handle_game(int client1, int client2){
  char buffer[BUFFER_SIZE];
  struct Player players[2] = {{1,3}, {1,3}};
  int turn = 1;

  snprintf(buffer,sizeof(buffer),"Game started\n");
  send(client1,buffer,strlen(buffer),0);
  send(client2,buffer,strlen(buffer),0);

  while(1){
    int active_client = (turn == 1) ? client1 : client2;
    int opponent_client = (turn == 1) ? client2 : client1;
    struct Player * active_player = &players[turn-1];
    struct Player * opponent_player = &players[2-turn];


    snprintf(buffer,sizeof(buffer),"Your turn. Lives: %d | Opponent Lives: %d\n Enter command (POSITION <0|1|2> or SHOOT <0|1|2>) (type 'quit' to exit): ",active_player->lives,opponent_player->lives);

    send(active_client,buffer,strlen(buffer),0);

    memset(buffer,0,sizeof(buffer));
    int bytes_read = recv(active_client,buffer,sizeof(buffer)-1,0);
    if(bytes_read<=0||strcmp(buffer,"quit\n")==0){
      snprintf(buffer,sizeof(buffer),"Player %d has left the game\n",turn);
      send(client1,buffer,strlen(buffer),0);
      send(client2,buffer,strlen(buffer),0);
      break;
    }

    buffer[bytes_read] = '\0';
    send(opponent_client,buffer,strlen(buffer),0);

    if(strncmp(buffer,"POSITION",8)==0){
      int new_position;
      if (sscanf(buffer+9,"%d",&new_position)==1 && new_position >= 0 && new_position <=2){
        active_player->position = new_position;
        snprintf(buffer,sizeof(buffer),"Player %d moved to position %d\n",turn,new_position);
        send(active_client,buffer,strlen(buffer),0);
      }else{
        snprintf(buffer,sizeof(buffer),"Invalid position. Use 0 (left), 1 (middle), or 2 (right)\n");
        send(active_client,buffer,strlen(buffer),0);
        continue;
      }
    }else if (strncmp(buffer,"SHOOT",5)==0){
      int target_position;
      if(sscanf(buffer+6,"%d",&target_position)==1 && target_position >=0 && target_position <= 2){
        if(target_position==opponent_player->position){
          opponent_player->lives--;
          snprintf(buffer,sizeof(buffer),"Hit! Player %d loses a life. Remaining lives: %d\n",(turn==1) ? 2 : 1,opponent_player->lives);
        }else{
          snprintf(buffer,sizeof(buffer),"Missed. Player %d is safe.\n", (turn==1) ? 2 : 1);
        }
        send(active_client,buffer,strlen(buffer),0);
        send(opponent_client,buffer,strlen(buffer),0);

        //for game over checking
        if(opponent_player->lives<=0){
          snprintf(buffer,sizeof(buffer), "Player %d wins! Game over.\n",turn);
          send(client1,buffer,strlen(buffer),0);
          send(client2,buffer,strlen(buffer),0);
          break;
        }
      }else{
        snprintf(buffer,sizeof(buffer),"Invalid target! Use 0 (left), 1 (middle), or 2 (right)\n");
        send(active_client,buffer,strlen(buffer),0);
        continue;
      }
    }else{
      snprintf(buffer,sizeof(buffer),"Invalid command! Use POSITION or SHOOT\n");
      send(active_client,buffer,strlen(buffer),0);
      continue;
    }
    //for alternating turns
    turn = 3-turn;
  }
  close(client1);
  close(client2);
  exit(0);
}

void handle_client(int client_socket){
  char buffer[BUFFER_SIZE];

  recv(client_socket,buffer,sizeof(buffer)-1,0);
  buffer[strcspn(buffer,"\n")] = '\0';
  printf("Client connected: %s\n",buffer);

  add_to_lobby(client_socket,buffer);

  while(1){
    //here, deal with disconnecting and inviting
    memset(buffer,0,sizeof(buffer));
    int bytes_read = recv(client_socket,buffer,sizeof(buffer)-1,0);
    if(bytes_read <= 0){
      printf("Client disconnected: %s\n",buffer);
      remove_from_lobby(client_socket);
      close(client_socket);
      break;
    }

    buffer[bytes_read] = '\0';

    if(strncmp(buffer,"INVITE",6)==0){
      char opponent_name[BUFFER_SIZE];
      sscanf(buffer+7,"%s",opponent_name);

      int opponent_socket = -1;
      for(int x=0;x<lobby_count;x++){
        if(strcmp(lobby[x].name,opponent_name)==0){
          opponent_socket = lobby[x].socket;
          break;
        }
      }

      if(opponent_socket!=-1){
        //send invite to opponent
        snprintf(buffer,sizeof(buffer),"%s invites you to a game. Type 'ACCEPT' to join.\n",lobby[0].name);
        send(opponent_socket,buffer,strlen(buffer),0);

        memset(buffer,0,sizeof(buffer));
        recv(opponent_socket,buffer,sizeof(buffer)-1,0);
        if(strncmp(buffer,"ACCEPT",6)==0){
          remove_from_lobby(client_socket);
          remove_from_lobby(opponent_socket);
          if(fork()==0){
            handle_game(client_socket,opponent_socket);
            add_to_lobby(client_socket,lobby[0].name);
            add_to_lobby(opponent_socket,opponent_name);
            exit(0);
          }
        }else{
          
        }
      }
    }
  }
}

int main(){
  struct addrinfo hints, *res;
  int listen_socket, client1, client2;

  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if(getaddrinfo(NULL,PORT,&hints,&res)!=0){
    perror("getaddrinfo failed");
    exit(1);
  }

  listen_socket = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
  if(listen_socket==-1){
    perror("socket failed");
    exit(1);
  }

  if(bind(listen_socket,res->ai_addr,res->ai_addrlen)==-1){
    perror("bind failed");
    close(listen_socket);
    exit(1);
  }

  freeaddrinfo(res);

  if(listen(listen_socket,MAX_CLIENTS)==-1){
    perror("listen failed");
    close(listen_socket);
    exit(1);
  }

  printf("Server listening on port %s...\n",PORT);

  while(1){
    printf("Waiting for Player 1\n");
    client1 = accept(listen_socket,NULL,NULL);
    if(client1==-1){
      perror("accept failed");
      continue;
    }
    printf("Player 1 connected\n");

    printf("Waiting for Player 2\n");
    client2 = accept(listen_socket,NULL,NULL);
    if(client2==-1){
      perror("accept failed");
      close(client1);
      continue;
    }

    printf("Player 2 connected\n");

    if(fork()==0){
      close(listen_socket);
      handle_game(client1,client2);
    }

    close(client1);
    close(client2);
  }
  close(listen_socket);
  return 0;
}

/*
struct Lobby lobby;
struct PlayerState players[MAX_PLAYERS];
int playercount = 0;

void initialize_lobby() {
    lobby.playercount = 0;
    sem_init(&lobby.lobbylock, 0, 1);
}

void add_to_lobby(struct PlayerState* player) {
    sem_wait(&lobby.lobbylock);
    if (lobby.playercount < MAX_PLAYERS) {
        players[playercount++] = *player;
        strncpy(lobby.players[lobby.playercount], player->name, MAX_NAME_LENGTH);
        lobby.playercount++;
    }
    sem_post(&lobby.lobbylock);
}

void remove_from_lobby(char* playername) {
    sem_wait(&lobby.lobbylock);
    for (int x = 0; x < lobby.playercount; x++) {
        if (strcmp(lobby.players[x], playername) == 0) {
            for (int y = x; y < lobby.playercount - 1; y++) {
                strncpy(lobby.players[y], lobby.players[y + 1], MAX_NAME_LENGTH);
            }
            lobby.playercount--;
            break;
        }
    }
    sem_post(&lobby.lobbylock);
}

void display_lobby(int clientfd) {
    sem_wait(&lobby.lobbylock);
    char buffer[256] = "Available players:\n";
    for (int x = 0; x < lobby.playercount; x++) {
        strcat(buffer, lobby.players[x]);
        strcat(buffer, "\n");
    }
    write(clientfd, buffer, strlen(buffer) + 1);
    sem_post(&lobby.lobbylock);
}

void notify_turn(int playerindex) {
    char clientpipe[50];
    sprintf(clientpipe, CLIENT_PIPE, players[playerindex].name);

    int clientfd = open(clientpipe, O_WRONLY);
    if (clientfd == -1) {
        perror("Failed to open client pipe for player notification\n");
        return;
    }

    char msg[] = "Your turn. Enter position (0: L, 1: M, 2: R) and shot (0: l, 1: m, 2: r):";
    write(clientfd, msg, sizeof(msg));
    close(clientfd);
}

void process_turn(int playerindex, int opponentindex) {
    char clientpipe[50];
    sprintf(clientpipe, CLIENT_PIPE, players[playerindex].name);

    int clientfd = open(clientpipe, O_RDONLY);
    if (clientfd == -1) {
        perror("Failed to open client pipe for player input\n");
        return;
    }

    struct Move move;
    if (read(clientfd, &move, sizeof(struct Move)) > 0) {
        printf("Player %s moved to position %d and shot at %d\n",
               players[playerindex].name, move.position, move.shot);

        // Update player position
        players[playerindex].position = move.position;

        // Check if the shot hits the opponent
        if (move.shot == players[opponentindex].position) {
            players[opponentindex].lives--;
            printf("Player %s hit Player %s! Remaining lives: %d\n",
                   players[playerindex].name, players[opponentindex].name,
                   players[opponentindex].lives);
        } else {
            printf("Player %s missed Player %s.\n",
                   players[playerindex].name, players[opponentindex].name);
        }
    } else {
        printf("Failed to read move from player %s\n", players[playerindex].name);
    }
    close(clientfd);
}

void game_loop() {
    while (players[0].lives > 0 && players[1].lives > 0) {
        // Player 1's turn
        notify_turn(0);
        process_turn(0, 1);

        if (players[1].lives <= 0) break;

        // Player 2's turn
        notify_turn(1);
        process_turn(1, 0);
    }

    printf("Game over! ");
    if (players[0].lives > 0) {
        printf("Player %s wins!\n", players[0].name);
    } else {
        printf("Player %s wins!\n", players[1].name);
    }
}

void sighandler(int signo) {
    printf("Shutting down server...\n");
    unlink(SERVER_PIPE);
    sem_destroy(&lobby.lobbylock);
    exit(0);
}

int main() {
    signal(SIGINT, sighandler);

    printf("Creating server...\n");

    if (mkfifo(SERVER_PIPE, 0666) == -1) {
        perror("Server: Failed to create server pipe\n");
        exit(1);
    }

    initialize_lobby();
    printf("Waiting for players...\n");

    while (1) {
        int serverfd = open(SERVER_PIPE, O_RDONLY);
        if (serverfd == -1) {
            perror("Server: Failed to open server pipe\n");
            exit(1);
        }

        struct PlayerState clientplayer;
        if (read(serverfd, &clientplayer, sizeof(struct PlayerState)) > 0) {
            add_to_lobby(&clientplayer);
            printf("Client %s connected\n", clientplayer.name);
        }
        close(serverfd);

        char clientpipe[50];
        sprintf(clientpipe, CLIENT_PIPE, clientplayer.name);

        int clientfd = open(clientpipe, O_WRONLY);
        if (clientfd == -1) {
            perror("Server: Failed to open client pipe\n");
            continue;
        }

        display_lobby(clientfd);

        char invite[MAX_NAME_LENGTH];
        read(serverfd, invite, sizeof(invite));
        printf("Player %s invited %s to play.\n", clientplayer.name, invite);

        char msg[100];
        sprintf(msg, "%s connection confirmed by server", clientplayer.name);
        write(clientfd, msg, sizeof(msg) + 1);
        close(clientfd);

        if (playercount == 2) {
            printf("Starting the game!\n");
            game_loop();
            break;
        }
    }

    unlink(SERVER_PIPE);
    sem_destroy(&lobby.lobbylock);
    return 0;
}
*/
