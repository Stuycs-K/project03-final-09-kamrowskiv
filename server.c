#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "shared.h"

int main(){
    printf("Creating server...\n");
    //Placeholder for server setup
    printf("Waiting for players\n");
    struct Player player1 = {"Player1", POS_MIDDLE, MAX_LIVES,0};
    struct Player player2 = {"Player2", POS_MIDDLE, MAX_LIVES,0};

    printf("Ready for connections\n");

    return 0;
}