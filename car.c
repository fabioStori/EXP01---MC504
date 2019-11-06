#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "car.h"
#include "directions.h"

Car *initCar(Car *c, int id, int dir, Car *nextCar) {
    c->id = id;
    c->direction = dir;
    c->nextCar = nextCar;
    c->atCrossing = false;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    c->condAtCrossing = cond;
    return c;
}


Car * addCar(Car *head, int id, int dir) {
    Car *current = head;

    while (current->nextCar != NULL) {
        current = current->nextCar;
    }
    Car *c = malloc(sizeof(Car));
    current->nextCar = initCar(c, id, dir, NULL);
    return c;
}


Car *popCar(Car *head) {
    struct Car *c = head->nextCar;
    head->nextCar = c->nextCar;
    return c;
}


int queueLen(Car *head) {
    int count = 0;
    Car *current = head;

    while (current->nextCar != NULL) {
        count++;
        current = current->nextCar;
    }
    return count;
}


ThreadData * carThreadArgs(Car * c, Car * queueHead){
    ThreadData * threadData = malloc(sizeof(ThreadData));
    threadData->c = c;
    threadData->queueHead = queueHead;
    return threadData;
}

void *carThread(void * args) {
    ThreadData *carThreadArgs = (ThreadData *) args;
    // Waits for an update, until this bat is the first of it's queue
    Car * c = carThreadArgs->c;
    Car * queueHead = carThreadArgs->queueHead;
    int direction = c->direction;

    pthread_cond_signal(&carThreadStarted);
    pthread_cond_wait(&c->condAtCrossing, &mutexUpdate);
    printf("BAT %d %c chegou no cruzamento\n", c->id, directionToChar(c->direction));
    c->atCrossing = true;

    pthread_cond_signal(&updateFinish);
    pthread_mutex_unlock(&mutexUpdate);

    pthread_cond_wait(&crossingTurn[direction], &mutexCrossingTurn[direction]);
    bool someoneElseWantsToCross = false;
    for(int i=0; i<4; i++){
        if(i != direction && crossingBitmask[i] == 1) {
            someoneElseWantsToCross = true;
            break;
        }
    }

    if (queueLen(queueHead) < K && someoneElseWantsToCross) {
        gaveTurnAway = true;
        pthread_cond_signal(&crossingCleared[direction]);
        pthread_cond_wait(&crossingTurn[direction], &mutexCrossingTurn[direction]);
    }
    gaveTurnAway = false;
    printf("BAT %d %c saiu no cruzamento\n", c->id, directionToChar(c->direction));
    free(popCar(queueHead));

    pthread_cond_signal(&crossingCleared[direction]);
    pthread_mutex_unlock(&mutexCrossingTurn[direction]);
    pthread_exit(0);
}
