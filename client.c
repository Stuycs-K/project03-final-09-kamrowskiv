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
    printf("Connected to server\n");

    printf("Enter your name: ");
    memset(buffer,0,sizeof(buffer));
    fgets(buffer,sizeof(buffer),stdin);
    buffer[strcspn(buffer,"\n")] = '\0';
    send(socket_fd,buffer,strlen(buffer),0);

    while(1){
        memset(buffer,0,sizeof(buffer));
        int bytes_read = recv(socket_fd,buffer,sizeof(buffer)-1,0);
        if(bytes_read<=0){
            printf("Disconnected from server\n");
            break;
        }

        buffer[bytes_read] = '\0';
        printf("%s",buffer);

        if(strstr(buffer,"invites you to a game")!=NULL){
            printf("Type 'ACCEPT' to join the game, or ignore to stay in the lobby: ");
            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, sizeof(buffer),stdin);
            send(socket_fd,buffer,strlen(buffer),0);
        }

        if(strstr(buffer,"Lobby: ")!=NULL){
          printf("Type 'INVITE <player_name>' to invite a player to a game: ");
          memset(buffer,0,sizeof(buffer));
          fgets(buffer,sizeof(buffer),stdin);
          send(socket_fd,buffer,strlen(buffer),0);
        }

        if(strstr(buffer,"Your turn")!=NULL){
            printf("Enter your command (POSITION <0|1|2> or SHOOT <0|1|2>): ");
            memset(buffer,0,sizeof(buffer));
            fgets(buffer,sizeof(buffer),stdin);
            send(socket_fd,buffer,strlen(buffer),0);

            if(strncmp(buffer,"quit",4)==0){
                printf("You quit the game\n");
                break;
            }
        }
    }
    close(socket_fd);
    return 0;
}
