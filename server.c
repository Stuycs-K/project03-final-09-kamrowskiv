#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>
#include "shared.h"

struct Lobby lobby;
struct PlayerState players[MAX_PLAYERS];
int playercount = 0;
int currentturn = 0;

void initialize_lobby(){
  lobby.playercount = 0;
  sem_init(&lobby.lobbylock,0,1);
}

void add_to_lobby(char*playername){
  sem_wait(&lobby.lobbylock);
  if(lobby.playercount<MAX_PLAYERS){
    players[playercount++] = *player;
    strncpy(lobby.players[lobby.playercount],player->name,MAX_NAME_LENGTH);
    lobby.playercount++;
  }
  sem_post(&lobby.lobbylock);
}

void remove_from_lobby(char*playername){
  sem_wait(&lobby.lobbylock);
  for(int x=0;x<lobby.playercount;x++){
    if(strcmp(lobby.players[x],playername)==0){
      for(int y=x;y<lobby.playercount-1;y++){
        strncpy(lobby.players[y],lobby.players[y+1],MAX_NAME_LENGTH);
      }
      lobby.playercount--;
      break;
    }
  }
  sem_post(&lobby.lobbylock);
}

void display_lobby(int clientfd){
  sem_wait(&lobby.lobbylock);
  char buffer[256] = "Avaliable players:\n";
  for(int x =0;x<lobby.playercount;x++){
    strcat(buffer, lobby.players[x]);
    strcat(buffer, "\n");
  }
  write(clientfd,buffer,strlen(buffer)+1);
  sem_post(&lobby.lobbylock);
}

void notify_turn(int playerindex){
  char clientpipe[50];
  sprintf(clientpipe, CLIENT_PIPE, players[playerindex].name);

  int clientfd = open(clientpipe, O_WRONLY);
  if(clientfd==-1){
    perror("Failed to open client pipe for player notification\n");
    return;
  }

  char msg[] = "Your turn. Enter position (L,M,R)";
  write(clientfd,msg,sizeof(msg));
  close(clientfd);
}

void process_turn(int playerindex){
  char clientpipe[50];
  sprintf(clientpipe,CLIENT_PIPE,players[playerindex].name);

  int clientfd = open(clientpipe, O_RDONLY);
  if(clientfd==-1){
    perror("Failed to open client pipe for player input\n");
    return;
  }

  char move[5];
  if(read(clientfd,move,sizeof(move))>0){
    printf("Player %s chose move: %s\n",players[playerindex].name,move);

    if(strcmp(move,"L")==0) players[playerindex].position = POS_LEFT;
    else if (strcmp(move,"M")==0) players[playerindex].position = POS_MIDDLE;
    else if (strcmp(move,"R")==0) players[playerindex].position = POS_RIGHT;
    else printf("Invalid move from player %s\n",players[playerindex].name);
}
close(clientfd);
}


int main(){
    printf("Creating server...\n");

    mkfifo(SERVER_PIPE,0666);
    initialize_lobby();
    printf("Waiting for players\n");
    while(1){
      int serverfd = open(SERVER_PIPE,O_RDONLY);
      if (serverfd==-1){
        perror("Server: Failed to open server pipe\n");
        exit(1);
      }

      struct PlayerState clientplayer;
      if(read(serverfd,&clientplayer,sizeof(struct PlayerState))>0){
        add_to_lobby(clientplayer.name);
        printf("Client %s connected\n",clientplayer.name);


      char clientpipe[50];
      sprintf(clientpipe, CLIENT_PIPE,clientplayer.name);
      //printf("opening pipe %s",clientpipe);
      int clientfd = open(clientpipe,O_WRONLY);
      if (clientfd==-1){
        perror("Server: Failed to open client pipe\n");
        continue;
      }

      display_lobby(clientfd);

      char invite[MAX_NAME_LENGTH];
      read(serverfd,invite,sizeof(invite));
      printf("Player %s invited %s to play.\n", clientplayer.name,invite);

      char msg[100];
      sprintf(msg,"%s connection confirmed by server",clientplayer.name);
      write(clientfd,msg,sizeof(msg)+1);
      close(clientfd);
      }
      close(serverfd);
    }

  unlink(SERVER_PIPE);
  sem_destroy(&lobby.lobbylock);
    return 0;
}
