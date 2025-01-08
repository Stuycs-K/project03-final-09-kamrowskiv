#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "shared.h"

int main(){
    printf("Connecting to server...\n");

    struct PlayerState player;
    player.position = POS_MIDDLE;
    player.lives = MAX_LIVES;
    player.score = 0;
    printf("What is your name? (Max 25 chars)\n");
    fgets(player.name,MAX_NAME_LENGTH,stdin);

    char clientpipe[50];
    sprintf(clientpipe,CLIENT_PIPE,player.name);
    if(mkfifo(clientpipe,0666)==-1){
        perror("Client: Failed to create client pipe\n");
        exit(1);
    }
    printf("Created client pipe %s\n",clientpipe);

    int serverfd = open(SERVER_PIPE, O_WRONLY);
    if (serverfd == -1) {
        perror("Client: Failed to open server pipe");
        unlink(clientpipe);
        exit(1);
    }
    write(serverfd, &player, sizeof(struct PlayerState));
    close(serverfd);
    printf("Client: Sent player data to server\n");

    int clientfd = open(clientpipe, O_RDONLY);
    if(clientfd==-1){
        perror("Client: Failed to open client pipe\n");
        unlink(clientpipe);
        exit(1);
    }

    char ack_msg[50];
    if(read(clientfd, ack_msg, sizeof(ack_msg))>0){
        printf("Client: Server says: %s\n",ack_msg);
    }
    close(clientfd);
    unlink(clientpipe);

    return 0;


}