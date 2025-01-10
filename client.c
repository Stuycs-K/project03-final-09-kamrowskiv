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
    struct addrinfo hints,*res;
    int socket_fd;
    char buffer[BUFFER_SIZE];

    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if(getaddrinfo("127.0.0.1",PORT,&hints,&res)!=0){
        perror("getaddrinfo failed");
        exit(1);
    }

    socket_fd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if(socket_fd==-1){
        perror("socket failed");
        exit(1);
    }

    if(connect(socket_fd,res->ai_addr,res->ai_addrlen)==-1){
        perror("connect failed");
        close(socket_fd);
        exit(1);
    }

    freeaddrinfo(res);
    printf("connected to server\n");

    while(1){
        memset(buffer,0,sizeof(buffer));
        int bytes_read = recv(socket_fd,buffer,sizeof(buffer)-1,0);
        if(bytes_read<=0){
            printf("Disconnected from server\n");
            break;
        }
        buffer[bytes_read] = '\0';
        printf("%s",buffer);

        if(strstr(buffer,"Your turn")!=NULL){
            printf("Enter your move: ");
            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, sizeof(buffer),stdin);

            send(socket_fd,buffer,strlen(buffer),0);

            if(strncmp(buffer,"quit",4)==0){
                printf("You quit\n");
                break;
            }
        }
    }
    close(socket_fd);
    return 0;
}

/*
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
*/