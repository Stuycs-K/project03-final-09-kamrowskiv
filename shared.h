#ifndef SHARED_H
#define SHARED_H

#define MAX_LIVES 3
#define POS_LEFT 0
#define POS_MIDDLE 1
#define POS_RIGHT 2
#define MAX_NAME_LENGTH 50

struct PlayerState{
    char name[MAX_NAME_LENGTH];
    int position; //Left, right, or middle based on defined position constants
    int lives; //Number of lives left
    int score; //For if multiple rounds played
}

#endif