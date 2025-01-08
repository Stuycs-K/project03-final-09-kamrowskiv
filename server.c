#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "shared.h"

int main(){
    printf("Creating server...\n");

    mkfifo(SERVER_PIPE,0666);
    printf("Waiting for players\n");
    while(1){
      int serverfd = open(SERVER_PIPE,O_RDONLY);
      if (serverfd==-1){
        perror("Server: Failed to open server pipe\n");
        exit(1);
      }

      struct PlayerState clientplayer;
      if(read(serverfd,&clientplayer,sizeof(struct PlayerState))>0){
        printf("Client %s connected\n",clientplayer.name);
      }
      close(serverfd);

      char clientpipe[50];
      sprintf(clientpipe, CLIENT_PIPE,clientplayer.name);
      printf("opening pipe %s",clientpipe);
      int clientfd = open(clientpipe,O_WRONLY);
      if (clientfd==-1){
        perror("Server: Failed to open client pipe\n");
        continue;
      }

      char msg[50];
      sprintf(msg,"%s connection confirmed by server",clientplayer.name);
      write(clientfd,msg,sizeof(msg)+1);
      close(clientfd);
    }

  unlink(SERVER_PIPE);
    return 0;
}
