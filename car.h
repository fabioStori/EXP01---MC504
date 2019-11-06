//
// Created by felipe.domingues on 10/5/19.
//

#ifndef BATMANAGER_CAR_H
#define BATMANAGER_CAR_H

#include <pthread.h>
#include <stdbool.h>

extern int crossingBitmask[4];

//extern pthread_mutex_t mutex;
extern pthread_mutex_t mutexUpdate;
extern pthread_cond_t update, notEmpty, updateFinish, startCarThread, carThreadStarted;
extern pthread_cond_t crossingTurn[4];
extern pthread_mutex_t mutexCrossingTurn[4];
extern pthread_cond_t crossingCleared[4];
extern bool gaveTurnAway;
extern pthread_mutex_t mutextPrint;
extern int K;

typedef struct Car {
    int id;
    int direction;
    struct Car *nextCar;
    bool atCrossing;
    pthread_cond_t condAtCrossing;
} Car;

typedef struct ThreadData {
    Car *c;
    Car *queueHead;
} ThreadData;

void *carThread(void * args);

Car * addCar(Car *head, int id, int dir);

void printQueue(Car *head);

void queueLength(Car *head);

Car *initCar(Car *c, int id, int dir, Car *nextCar);

ThreadData * carThreadArgs(Car * c, Car * queueHead);

#endif //BATMANAGER_CAR_H

