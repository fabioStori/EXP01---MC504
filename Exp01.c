#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//https://gist.github.com/rtv/4989304

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int bitmask[4];
int N, E, S, W = 0;

//struct car pra lista de prioridade First In First Out de cada trilho
typedef struct car{
	int carId;
	char carDirection;
	struct car *nextCar;
}car;

void* threadFunc(void *arg);
void printQueue(car * head);
void addCar(car * head, int id, char dir);
void removeCar(car * head);

//função que toda thread ira rodar
//atualmente eu travo o mutex apenas pra printar os carros na ordem que eles chegam
void* threadFunc (void *head) {
	car * current = head;	
	while(current->nextCar != NULL){		
		pthread_mutex_lock(&mutex);
		switch (current->carDirection) {
			case 'n':
				N = 1;	
				break;
			case 'e':
				E = 1;
				break;
			case 's':
				S = 1;
				break;
			case 'w':
				W = 1;
				break;
		}
		pthread_cond_signal(&cond);		
		printQueue(head);
		current = current->nextCar;
		pthread_mutex_unlock(&mutex);
	}	
 	pthread_exit(NULL);
}

//imprime o ID e a DIREÇÃO do carro (BAT) na ordem "mais velho primeiro"
void printQueue(car * head) {
    car * temp = head->nextCar;
	printf("BAT %d %c chegou no cruzamento\n", temp->carId, temp->carDirection);
    // while (temp != NULL) {
    //     printf("BAT %d DIRECAO %c\n", temp->carId, temp->carDirection);
	// 	fflush(stdout);
	// 	sleep(1);
    //     temp = temp->nextCar;
    // }
}

//adiciona carro no queue First In First out da trilha
void addCar(car * head, int id, char dir) {
    car * current = head;

    while (current->nextCar != NULL) {
        current = current->nextCar;
    }

    current->nextCar = malloc(sizeof(car));
    current->nextCar->carId = id;
	current->nextCar->carDirection = dir;
    current->nextCar->nextCar = NULL;
}

void removeCar(car * head) {
	struct car *temp = head->nextCar;
    head->nextCar = temp;
	free(temp);
}


int main() {
	
	char input[100];
	scanf("%s", input);	

	for(int i=0; i<4; i++)
		bitmask[i] = 0;	

	//iniciando as listas ligadas (não lembro se tem uma forma mais bonita de fazer isso)
	car * headOfNorth = NULL;
	headOfNorth = malloc(sizeof(car));
	headOfNorth->nextCar = NULL;
	headOfNorth->carId = 0;
	headOfNorth->carDirection = 'n';

	car * headOfEast = NULL;
	headOfEast = malloc(sizeof(car));
	headOfEast->nextCar = NULL;
	headOfEast->carId = 0;
	headOfEast->carDirection = 'e';

	car * headOfSouth = NULL;
	headOfSouth = malloc(sizeof(car));
	headOfSouth->nextCar = NULL;
	headOfSouth->carId = 0;
	headOfSouth->carDirection = 's';

	car * headOfWest = NULL;
	headOfWest = malloc(sizeof(car));
	headOfWest->nextCar = NULL;
	headOfWest->carId = 0;
	headOfWest->carDirection = 'w';	
	
	car * queue = NULL;
	queue = malloc(sizeof(car));
	queue->nextCar = NULL;
	queue->carId = 0;
	queue->carDirection = ' ';	

	//para cada letra do input, cria um carro e adiciona ele na queue da trilha dele
	for (int i = 0; i < strlen(input); i++) {
		switch (input[i]) {
			case 'n':
				addCar(headOfNorth, i+1, 'n');	
				break;
			case 'e':
				addCar(headOfEast, i+1, 'e');
				break;
			case 's':
				addCar(headOfSouth, i+1, 's');
				break;
			case 'w':
				addCar(headOfWest, i+1, 'w');
				break;
		}		
	}

	//iniciando threads, passando o threadFunc e a cabeça da lista de prioridades da respectiva trilha
	//to criando uma thread pra cada trilha, da forma que conversamos...

	pthread_t threadNorth, threadEast, threadSouth, threadWest;
	int retN, retE, retS, retW;
	retN = pthread_create(&threadNorth, NULL, threadFunc, (void *) headOfNorth);
	retE = pthread_create(&threadEast, NULL, threadFunc, (void *) headOfEast);
	retS = pthread_create(&threadSouth, NULL, threadFunc, (void *) headOfSouth);
	retW = pthread_create(&threadWest, NULL, threadFunc, (void *) headOfWest);

	pthread_mutex_lock(&mutex);
	while(N==1 || E==1 || S==1 || W==1){
		char aut = 'z';
		char notAut = 'z';

		pthread_cond_wait(&cond, &mutex);

		bitmask[0]=N;
		bitmask[1]=E;
		bitmask[2]=S;
		bitmask[3]=W;

		int sum=0;

		for(int i=0; i<4; i++)
			sum = sum + bitmask[i];

		printf("entrou no while");

		int impasse = 0;

		if(sum > 1){
			printf("entrou no if do main");

			for(int i=0; i<4 && impasse != 2; i++){
				if(bitmask[i] == 1 && impasse == 0){
					switch(i){
						case 0:
							aut = headOfNorth->nextCar->carDirection;
							bitmask[i] = 0;
							removeCar(headOfNorth);
						case 1:
							aut = headOfEast->nextCar->carDirection;
							bitmask[i] = 0;
							removeCar(headOfEast);
						case 2:
							aut = headOfSouth->nextCar->carDirection;
							bitmask[i] = 0;
							removeCar(headOfSouth);
						case 3:
							aut = headOfWest->nextCar->carDirection;
							bitmask[i] = 0;
							removeCar(headOfWest);
					}
					impasse++;
				}else if(bitmask[i] == 1 && impasse == 1){
					switch(i){
						case 0:
							notAut = headOfNorth->nextCar->carDirection;
						case 1:
							notAut = headOfEast->nextCar->carDirection;
						case 2:
							notAut = headOfSouth->nextCar->carDirection;
						case 3:
							notAut = headOfWest->nextCar->carDirection;
					}
					impasse++;
				}
				
			}

			printf("Impasse: %c e %c , sinalizando %c para ir", aut, notAut, aut);
		}
	}
		
	pthread_mutex_unlock(&mutex);

	pthread_join(threadNorth, NULL);
	pthread_join(threadEast, NULL);
	pthread_join(threadSouth, NULL);
	pthread_join(threadWest, NULL);

	//retorno da thread... acho que '0' é bom, diferente disso é ruim
	printf("thread N retornou %d\n", retN);
	printf("thread E retornou %d\n", retE);
	printf("thread S retornou %d\n", retS);
	printf("thread W retornou %d\n", retW);

	// printQueue(headOfNorth);	
	// printQueue(headOfEast);	
	// printQueue(headOfSouth);	
	// printQueue(headOfWest);
	return (0);
}
