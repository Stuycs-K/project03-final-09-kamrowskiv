#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "shared.h"

int main(){
    printf("Connecting to server...\n");
    //Placeholder, connection setup done here
    printf("Connected!\n");

    struct PlayerState player;
    player.position = POS_MIDDLE;
    player.lives = MAX_LIVES;
    player.score = 0;
    printf("What is your name? (Max 25 chars)\n");
    fgets(player.name,MAX_NAME_LENGTH,stdin);

    int serverfd;
    serverfd = open(SERVER_PIPE,O_WRONLY);
    if(serverfd==-1){
        perror("Client: Failed to open server pipe\n");
        exit(1);
    }

    write(serverfd,&player,sizeof(struct PlayerState));
    close(serverfd);

    char clientpipe[20];
    sprintf(clientpipe,CLIENT_PIPE,player.name);
    mkfifo(clientpipe,0666);

    int clientfd = open(clientpipe, O_RDONLY);
    if(clientfd==-1){
        perror("Client: Failed to open client pipe\n");
        exit(1);
    }

    char ack_msg[50];
    read(clientfd, ack_msg, sizeof(ack_msg));
    printf("Client: Server says: %s\n",ack_msg);
    close(clientfd);
    unlink(clientpipe);

    return 0;


}
