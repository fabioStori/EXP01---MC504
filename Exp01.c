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
typedef struct car {
	int carId;
	char carDirection;
	struct car *nextCar;
}car;

void* threadFunc(void *arg);
void addCar(car * head, int id, char dir);
void removeCar(car * head);

//função que toda thread ira rodar
void* threadFunc(void *head) {
	car * current = head;
	while (current->nextCar != NULL) {
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
		printf("BAT %d %c chegou no cruzamento\n", current->nextCar->carId, current->nextCar->carDirection);
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);

		current = current->nextCar;		
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
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
	return;
}

void removeCar(car * head) {	
	struct car *temp = head->nextCar;
	head->nextCar = temp->nextCar;
	free(temp);
	return;
}


int main() {

	char input[100];
	scanf("%s", input);

	for (int i = 0; i < 4; i++)
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

	//para cada letra do input, cria um carro e adiciona ele na queue da trilha dele
	for (int i = 0; i < strlen(input); i++) {
		switch (input[i]) {
		case 'n':
			addCar(headOfNorth, i + 1, 'n');
			break;
		case 'e':
			addCar(headOfEast, i + 1, 'e');
			break;
		case 's':
			addCar(headOfSouth, i + 1, 's');
			break;
		case 'w':
			addCar(headOfWest, i + 1, 'w');
			break;
		}
	}
	//iniciando threads, passando o threadFunc e a cabeça da lista de prioridades da respectiva trilha
	//to criando uma thread pra cada trilha, da forma que conversamos...

	pthread_t threadNorth, threadEast, threadSouth, threadWest;
	int retN, retE, retS, retW;
	int threadsInitialized = 0;	
	int sum = 0;
	int whileIt = 0;

	pthread_mutex_lock(&mutex);
	do{
		whileIt++;
		if (!threadsInitialized) {
			retN = pthread_create(&threadNorth, NULL, threadFunc, (void *)headOfNorth);
			retE = pthread_create(&threadEast, NULL, threadFunc, (void *)headOfEast);
			retS = pthread_create(&threadSouth, NULL, threadFunc, (void *)headOfSouth);
			retW = pthread_create(&threadWest, NULL, threadFunc, (void *)headOfWest);
			threadsInitialized = 1;
		}

		pthread_cond_wait(&cond, &mutex);

		//atualizando bitmask
		bitmask[0] = N;
		bitmask[1] = E;
		bitmask[2] = S;
		bitmask[3] = W;

		sum = 0;
		for (int i = 0; i < 4; i++)
			sum = sum + bitmask[i];
		
		
		int foundHighestPriority = 0;
		
		car * authorizedCar = NULL;
		authorizedCar= malloc(sizeof(car));
		authorizedCar->carId = 0;
		authorizedCar->carDirection = 'z';
		authorizedCar->nextCar = NULL;

		if (sum > 1) {
			printf("Impasses: ");
			for (int i = 0; i < 4; i++) {
				if (bitmask[i] == 1) {
					switch (i) {
					case 0:
						printf("[%c] ", headOfNorth->nextCar->carDirection);
						if (!foundHighestPriority) {

							authorizedCar->carDirection = headOfNorth->nextCar->carDirection;
							authorizedCar->carId = headOfNorth->nextCar->carId;

							foundHighestPriority = 1;
							N = 0;
							bitmask[i] = 0;
							removeCar(headOfNorth);
						}
						
						break;
					case 1:
						printf("[%c] ", headOfEast->nextCar->carDirection);
						if (!foundHighestPriority) {

							authorizedCar->carDirection = headOfEast->nextCar->carDirection;
							authorizedCar->carId = headOfEast->nextCar->carId;

							foundHighestPriority = 1;
							E = 0;
							bitmask[i] = 0;
							removeCar(headOfEast);
						}
						break;
					case 2:
						printf("[%c] ", headOfSouth->nextCar->carDirection);
						if (!foundHighestPriority) {

							authorizedCar->carDirection = headOfSouth->nextCar->carDirection;
							authorizedCar->carId = headOfSouth->nextCar->carId;

							foundHighestPriority = 1;
							S = 0;
							bitmask[i] = 0;
							removeCar(headOfSouth);
						}
						break;
					case 3:
						printf("[%c] ", headOfWest->nextCar->carDirection);
						if (!foundHighestPriority) {

							authorizedCar->carDirection = headOfWest->nextCar->carDirection;
							authorizedCar->carId = headOfWest->nextCar->carId;

							foundHighestPriority = 1;
							W = 0;
							bitmask[i] = 0;
							removeCar(headOfWest);
						}
						break;
					}
				}				
			}
			printf(". Sinalizando para %c ir\n", authorizedCar->carDirection);
			printf("BAT %d %c saiu no cruzamento\n", authorizedCar->carId, authorizedCar->carDirection);
		}
		else if (sum == 1) {
			for (int i = 0; i < 4; i++) {
				if (bitmask[i] == 1) {
					switch (i) {
					case 0:
						printf("BAT %d %c saiu do cruzamento\n", headOfNorth->nextCar->carId, headOfNorth->nextCar->carDirection);
						N = 0;
						bitmask[i] = 0;
						removeCar(headOfNorth);
						break;
					case 1:
						printf("BAT %d %c saiu do cruzamento\n", headOfEast->nextCar->carId, headOfEast->nextCar->carDirection);
						E = 0;
						bitmask[i] = 0;
						removeCar(headOfEast);
						break;
					case 2:
						printf("BAT %d %c saiu do cruzamento\n", headOfSouth->nextCar->carId, headOfSouth->nextCar->carDirection);
						S = 0;
						bitmask[i] = 0;
						removeCar(headOfSouth);
						break;
					case 3:
						printf("BAT %d %c saiu do cruzamento\n", headOfWest->nextCar->carId, headOfWest->nextCar->carDirection);
						W = 0;
						bitmask[i] = 0;
						removeCar(headOfWest);
						break;
					}
				}
			}
		}
		else {
			printf("Queue Vazio!\n");
		}
		
		free(authorizedCar);
		pthread_cond_signal(&cond);
	} while (sum!=0);

	pthread_mutex_unlock(&mutex);

	printf("iterações do while main %d\n", whileIt);

	free(headOfNorth);
	free(headOfEast);
	free(headOfSouth);
	free(headOfWest);
	

	pthread_join(threadNorth, NULL);
	pthread_join(threadEast, NULL);
	pthread_join(threadSouth, NULL);
	pthread_join(threadWest, NULL);

	//retorno da thread... acho que '0' é bom, diferente disso é ruim
	//printf("thread N retornou %d\n", retN);
	//printf("thread E retornou %d\n", retE);
	//printf("thread S retornou %d\n", retS);
	//printf("thread W retornou %d\n", retW);

	// printQueue(headOfNorth);	
	// printQueue(headOfEast);	
	// printQueue(headOfSouth);	
	// printQueue(headOfWest);
	return (0);
}
