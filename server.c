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
      for(int y=x;y<lobby_count-1;y++){
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
          snprintf(buffer,sizeof(buffer),"Invitation declined by %s\n",opponent_name);
          send(client_socket,buffer,strlen(buffer),0);
        }
      }else{
        snprintf(buffer,sizeof(buffer),"Player %s not found in lobby\n",opponent_name);
        send(client_socket,buffer,strlen(buffer),0);
      }
    }else{
      snprintf(buffer,sizeof(buffer),"Invalid command. Use INVITE <player_name>\n");
      send(client_socket,buffer,strlen(buffer),0);
    }
  }
}

int main(){
  struct addrinfo hints, *res;
  int listen_socket;

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
    int client_socket = accept(listen_socket,NULL,NULL);
    if(client_socket==-1){
      perror("accept failed");
      continue;
    }

    if(fork()==0){
      close(listen_socket);
      handle_client(client_socket);
      exit(0);
    }

    close(client_socket);
  }
  close(listen_socket);
  return 0;
}