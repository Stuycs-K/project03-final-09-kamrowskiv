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

  if(listen(listen_socket,2)==-1){
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

  }
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
