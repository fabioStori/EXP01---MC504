//FABIO STORI - 196631
//FELIPE DUARTE DOMINGUES - 171036
//MC 504 - EXPERIMENTO 01 - BATMAN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#include "car.h" // From the project
#include "directions.h"


int err;
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutexProcessQueue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexUpdate = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutexCrossingTurn[4] = {PTHREAD_MUTEX_INITIALIZER,
                                        PTHREAD_MUTEX_INITIALIZER,
                                        PTHREAD_MUTEX_INITIALIZER,
                                        PTHREAD_MUTEX_INITIALIZER};

pthread_cond_t update = PTHREAD_COND_INITIALIZER;
pthread_cond_t startCarThread = PTHREAD_COND_INITIALIZER;
pthread_cond_t carThreadStarted = PTHREAD_COND_INITIALIZER;
pthread_cond_t updateFinish = PTHREAD_COND_INITIALIZER;
pthread_cond_t notEmpty = PTHREAD_COND_INITIALIZER;
pthread_cond_t crossingTurn[4] = {PTHREAD_COND_INITIALIZER,
                                  PTHREAD_COND_INITIALIZER,
                                  PTHREAD_COND_INITIALIZER,
                                  PTHREAD_COND_INITIALIZER};
pthread_cond_t crossingCleared[4] = {PTHREAD_COND_INITIALIZER,
                                     PTHREAD_COND_INITIALIZER,
                                     PTHREAD_COND_INITIALIZER,
                                     PTHREAD_COND_INITIALIZER};
char input[100];
Car *queues[4];

int crossingBitmask[4];

bool batman_running = true;

int K = 2; // valor default
int id_counter = 1; // incremented indefinitely
bool gaveTurnAway = false;


void printBitmask(){
    printf("bitmask");
    for (int i = 0; i < 4; i++)
        printf("[%d]", crossingBitmask[i]);
    printf("\n");

}

void updateBitmask() {
    for (int i = 0; i < 4; i++) {
        if (queues[i]->nextCar == NULL)
            crossingBitmask[i] = 0;
        else
            crossingBitmask[i] = 1;
    }
    //printBitmask();
}

bool allQueuesEmpty() {
    for (int i = 0; i < 4; i++) {
        if (queues[i]->nextCar != NULL)
            return false;
    }
    return true;
}

void *batman() {
    while (batman_running) {
        pthread_cond_wait(&notEmpty, &mutexProcessQueue);
        do {
            updateBitmask();

            for (int i = 0; i < 4; i++) {
                if (crossingBitmask[i] == 1) {
                    if(!queues[i]->nextCar->atCrossing){

                        pthread_mutex_lock(&mutexUpdate);
                        pthread_cond_signal(&queues[i]->nextCar->condAtCrossing);

                        pthread_cond_wait(&updateFinish, &mutexUpdate);
                        pthread_mutex_unlock(&mutexUpdate);
                    }
                }
            }

            // verificar qual o impasse no bitmask e printá-lo
            int count = 0;
            int dirToGo = -1;
            for (int i = 0; i < 4; i++) {
                if (crossingBitmask[i] == 1) {
                    count++;
                }
            }

            if (count == 0) {
                printf("Nenhum carro no cruzamento.");
                break;
            }
            if (count > 1) {
                printf("Impasse: ");
                for (int i = 0; i < 4; i++) {
                    if (crossingBitmask[i] == 1)
                        printf("%c,", directionToChar(i));
                }
                for (int i = 0; i < 4; i++) {
                    if (crossingBitmask[i] == 1) {
                        printf(" sinalizando %c para ir\n", directionToChar(i));
                        break;
                    }
                }
            }

            for (int i = 0; i < 4; i++) {
                if (crossingBitmask[i] == 1) {
                    dirToGo = i;
                    break;
                }
            }

            do {
                // signals BAT to go
                pthread_mutex_lock(&mutexCrossingTurn[dirToGo]);
                pthread_cond_signal(&crossingTurn[dirToGo]);
                // waits for BAT to clear the crossing
                pthread_cond_wait(&crossingCleared[dirToGo], &mutexCrossingTurn[dirToGo]);
                pthread_mutex_unlock(&mutexCrossingTurn[dirToGo]);
                updateBitmask();
                if (gaveTurnAway) {
                    // procurar proxima direcao de prioridade e setar como dirToGo;
                    int next = dirToGo;
                    if (next == 3)
                        next = 0;

                    while (true) {
                        next++;
                        if (next == 3)
                            next = 0;
                        if (crossingBitmask[next] == 1) {
                            printf("BAT %d %c cedeu a passagem para BAT %d %c\n",
                                    queues[dirToGo]->nextCar->id, directionToChar(dirToGo),
                                    queues[next]->nextCar->id, directionToChar(next));
                            dirToGo = next;
                            break;
                        }
                    }
                }
            } while (gaveTurnAway);

        } while (!allQueuesEmpty());
    }
}

int main() {
    // Creating batman thread
    pthread_t batmanThread;
    err = pthread_create(&batmanThread, NULL, batman, NULL);
    if (err != 0) {
        printf("Couldn't create batman thread\n");
        return 1;
    }

    // Allocating and initializing queues
    for (int i = 0; i < 4; i++) {
        Car *queue = malloc(sizeof(Car));
        queues[i] = initCar(queue, -1, i, NULL);
    }

    while (true) {
        scanf("%s", input);

        if (strncmp(input, "K=", 2) == 0) {
            K = atoi(input + 2);  // pointer arithmetic, skipping the first two characters
            continue;
        }
        int d;

        for (long i = 0; i < strlen(input); i++) {
            switch (input[i]) {
                case 'n':
                case 'e':
                case 's':
                case 'w':
                    d = charToDirection(input[i]);
                    Car *c = addCar(queues[d], id_counter++, d);
                    pthread_t t;

                    pthread_mutex_lock(&mutexUpdate);
                    pthread_create(&t, NULL, carThread, carThreadArgs(c, queues[d]));
                    pthread_cond_wait(&carThreadStarted, &mutexUpdate);
                    pthread_mutex_unlock(&mutexUpdate);

            }
        }
        pthread_mutex_lock(&mutexProcessQueue);
        pthread_cond_signal(&notEmpty); // wakes up batman
        pthread_mutex_unlock(&mutexProcessQueue);

        if (strncmp(input, "Q", 0)) {
            printf("Stopping BATMan\n");
            break;
        }
    }


    return (0);
}
