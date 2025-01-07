#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
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
    fgets(player.name,50,stdin);


    printf("Player starts game with %d lives\n",player.lives);
    return 0;


}
