//
// Created by felipe.domingues on 10/5/19.
//
#include "directions.h"
char directionToChar(int direction){
    switch (direction){
        case 0:
            return 'N';
        case 1:
            return 'E';
        case 2:
            return 'S';
        case 3:
            return 'W';
        default:
            return 0;
    }
}

int charToDirection(char direction){
    switch (direction){
        case 'n':
        case 'N':
            return NORTH;
        case 'e':
        case 'E':
            return EAST;
        case 's':
        case 'S':
            return SOUTH;
        case 'w':
        case 'W':
            return WEST;
        default:
            return 0;
    }
}