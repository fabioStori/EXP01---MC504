#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//struct car pra lista de prioridade First In First Out de cada trilho
typedef struct car{
	int carId;
	char carDirection;
	struct car *nextCar;
}car;

void* threadFunc(void *arg);
void printQueue(car * head);
void addCar(car * head, int id, char dir);

//função que toda thread ira rodar
//atualmente eu travo o mutex apenas pra printar os carros na ordem que eles chegam
void* threadFunc (void *head) {	
	pthread_mutex_lock(&mutex);	

	printQueue(head);	

	pthread_mutex_unlock(&mutex);	
 	pthread_exit(NULL);
}

//imprime o ID e a DIREÇÃO do carro (BAT) na ordem "mais velho primeiro"
void printQueue(car * head) {
    car * current = head->nextCar;

    while (current != NULL) {
        printf("BAT %d DIRECAO %c\n", current->carId, current->carDirection);
	fflush(stdout);
	sleep(1);
        current = current->nextCar;
    }
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



int main() {
	
	char input[100];
	scanf("%s", input);	

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



	pthread_join(threadNorth, NULL);
	pthread_join(threadEast, NULL);
	pthread_join(threadSouth, NULL);
	pthread_join(threadWest, NULL);

	//retorno da thread... acho que '0' é bom, diferente disso é ruim
	printf("thread N retornou %d", retN);
	printf("thread E retornou %d", retE);
	printf("thread S retornou %d", retS);
	printf("thread W retornou %d", retW);



	// printQueue(headOfNorth);	
	// printQueue(headOfEast);	
	// printQueue(headOfSouth);	
	// printQueue(headOfWest);
	return (0);
}
