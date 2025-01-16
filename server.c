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
#include <sys/mman.h>
#include <sys/wait.h>
#include <netdb.h>
#include <errno.h>

#define PORT "9999"
#define BUFFER_SIZE 256
#define MAX_CLIENTS 10


struct Player{
  int position;
  int lives;
};

struct Client {
    int socket;
    char name[BUFFER_SIZE];
};

struct Client *lobby[MAX_CLIENTS];
int *lobby_count;


void broadcast_lobby() {
    char buffer[BUFFER_SIZE] = "Lobby: ";
    printf("Broadcasting lobby...\n");
    for (int i = 0; i < *lobby_count; i++) {
        strcat(buffer, lobby[i]->name);
        strcat(buffer, " ");
    }
    strcat(buffer, "\nType 'INVITE <player_name>' to invite a player or 'QUIT' to leave.\n");


    for (int i = 0; i < *lobby_count; i++) {
        printf("Sending lobby update to %s\n", lobby[i]->name);
        if (send(lobby[i]->socket, buffer, strlen(buffer), 0) == -1) {
            perror("Failed to broadcast lobby update");
        }
    }
}


void add_to_lobby(int client_socket, const char *name) {
    printf("Adding client to lobby: %s\n", name);
    if (*lobby_count < MAX_CLIENTS) {
        lobby[*lobby_count]->socket = client_socket;
        strncpy(lobby[*lobby_count]->name, name, BUFFER_SIZE);
        *lobby_count = *lobby_count +1;
        printf("Lobby size after adding: %d\n", *lobby_count);
        broadcast_lobby();
    } else {
        char msg[] = "Lobby is full, try again later.\n";
        send(client_socket, msg, strlen(msg), 0);
        close(client_socket);
    }
}


void remove_from_lobby(int client_socket) {
    printf("Removing client from lobby...\n");
    for (int i = 0; i < *lobby_count; i++) {
        if (lobby[i]->socket == client_socket) {
            printf("Client found: %s\n", lobby[i]->name);
            for (int j = i; j < *lobby_count - 1; j++) {
                *lobby[j] = *lobby[j + 1];
            }
            *lobby_count = *lobby_count - 1;
            printf("Lobby size after removal: %d\n", *lobby_count);
            broadcast_lobby();
            return;
        }
    }
    printf("Client not found in lobby.\n");
}


void handle_game(int client1, int client2) {
    char buffer[BUFFER_SIZE];
    int turn = 1;

    snprintf(buffer, sizeof(buffer), "Game started! Player 1 vs Player 2.\n");
    send(client1, buffer, strlen(buffer), 0);
    send(client2, buffer, strlen(buffer), 0);

    while (1) {
        int active_client = (turn == 1) ? client1 : client2;
        int opponent_client = (turn == 1) ? client2 : client1;

        snprintf(buffer, sizeof(buffer), "Your turn. Type 'POSITION <0|1|2>' or 'SHOOT <0|1|2>' (type 'quit' to leave): ");
        printf("Sending turn prompt to Player %d\n", turn);
        send(active_client, buffer, strlen(buffer), 0);

        memset(buffer, 0, sizeof(buffer));
        int bytes_read = recv(active_client, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0 || strncmp(buffer, "quit", 4) == 0) {
            snprintf(buffer, sizeof(buffer), "Player %d has left the game.\n", turn);
            printf("Player %d quit the game.\n", turn);
            send(client1, buffer, strlen(buffer), 0);
            send(client2, buffer, strlen(buffer), 0);
            break;
        }

        buffer[bytes_read] = '\0';
        printf("Player %d sent: %s\n", turn, buffer);
        send(opponent_client, buffer, strlen(buffer), 0);

        turn = 3 - turn;
    }

    close(client1);
    close(client2);
    exit(0);
}


void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];


    recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    buffer[strcspn(buffer, "\n")] = '\0';
    printf("Client connected: %s\n", buffer);
    add_to_lobby(client_socket, buffer);


    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if (bytes_read <= 0) {
            printf("Client disconnected: %s\n", buffer);
            remove_from_lobby(client_socket);
            close(client_socket);
            break;
        }

        buffer[bytes_read] = '\0';
        printf("Received command from : %s\n", buffer);


        if (strncmp(buffer, "INVITE", 6) == 0) {
            char opponent_name[BUFFER_SIZE];
            sscanf(buffer + 7, "%s", opponent_name);

            int opponent_socket = -1;
            for (int i = 0; i < *lobby_count; i++) {
                if (strcmp(lobby[i]->name, opponent_name) == 0) {
                    opponent_socket = lobby[i]->socket;
                    break;
                }
            }

            if (opponent_socket != -1) {
                snprintf(buffer, sizeof(buffer), "%s invites you to a game. Type 'ACCEPT' to join.\n", lobby[0]->name);
                send(opponent_socket, buffer, strlen(buffer), 0);

                memset(buffer, 0, sizeof(buffer));
                recv(opponent_socket, buffer, sizeof(buffer) - 1, 0);
                if (strncmp(buffer, "ACCEPT", 6) == 0) {
                    remove_from_lobby(client_socket);
                    remove_from_lobby(opponent_socket);
                    if (fork() == 0) {
                        handle_game(client_socket, opponent_socket);
                        exit(0);
                    }
                } else {
                    snprintf(buffer, sizeof(buffer), "Invitation declined by %s.\n", opponent_name);
                    send(client_socket, buffer, strlen(buffer), 0);
                }
            } else {
                snprintf(buffer, sizeof(buffer), "Player %s not found in the lobby.\n", opponent_name);
                send(client_socket, buffer, strlen(buffer), 0);
            }
        } else if (strncmp(buffer, "QUIT", 4) == 0) {
            printf("Client left the lobby: %s\n", buffer);
            remove_from_lobby(client_socket);
            close(client_socket);
            break;
        } else {
            snprintf(buffer, sizeof(buffer), "Invalid command. Type 'INVITE <player_name>' or 'QUIT'.\n");
            send(client_socket, buffer, strlen(buffer), 0);
        }
    }
}


int main() {
  lobby_count = (int*)mmap(
        NULL, sizeof(int), PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        *lobby_count = 0;
        printf("for lc: %d",*lobby_count);
      lobby = (struct Client **)mmap(NULL,sizeof(struct Client),PROT_READ | PROT_WRITE,MAP_SHARED | MAP_ANONYMOUS,-1,0);




    struct addrinfo hints, *res;
    int listen_socket;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) {
        perror("getaddrinfo failed");
        exit(1);
    }

    listen_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listen_socket == -1) {
        perror("socket failed");
        exit(1);
    }

    if (bind(listen_socket, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind failed");
        close(listen_socket);
        exit(1);
    }

    freeaddrinfo(res);

    if (listen(listen_socket, MAX_CLIENTS) == -1) {
        perror("listen failed");
        close(listen_socket);
        exit(1);
    }

    printf("Server listening on port %s...\n", PORT);

    while (1) {
        int client_socket = accept(listen_socket, NULL, NULL);
        if (client_socket == -1) {
            perror("accept failed");
            continue;
        }

        if (fork() == 0) {
            close(listen_socket);
            handle_client(client_socket);
            exit(0);
        }

        close(client_socket);
    }
    close(listen_socket);
    return 0;
}
