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
    player.name[strcspn(player.name,"\n")] = '\0';

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

    char buffer[256];
    read(clientfd,buffer,sizeof(buffer));
    printf("%s\n",buffer);

    printf("Enter player name to invite: ");
    char invite[MAX_NAME_LENGTH];
    fgets(invite,MAX_NAME_LENGTH,stdin);
    invite[strcspn(invite,"\n")] = '\0';

    serverfd = open(SERVER_PIPE,O_WRONLY);
    write(serverfd,invite,sizeof(invite));
    close(serverfd);

    read(clientfd,buffer,sizeof(buffer));
    printf("%s\n",buffer);
    
    close(clientfd);
    unlink(clientpipe);

    return 0;


}