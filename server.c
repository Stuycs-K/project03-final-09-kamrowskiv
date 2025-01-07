#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "shared.h"

int main(){
    printf("Creating server...\n");
    //Placeholder for server setup
    mkfifo(SERVER_PIPE,0666);
    printf("Waiting for players\n");
    while(1){
      int serverfd = open(SERVER_PIPE,O_RDONLY);

      struct PlayerState clientplayer;
      read(serverfd,&clientplayer,sizeof(struct PlayerState));
    }

    printf("Ready for connections\n");

    return 0;
}
