#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int counter = 0;

#define FALSE 0
#define TRUE 1
#define N 2

int turn = 0;
int interested[N] = {FALSE, FALSE};

void enter_region(int process) {
	int other = 1 - process;
	
	interested[process] = TRUE;
	turn = process;
	
	while(turn == process && interested[other] == TRUE) {pthread_yield();};
}

void leave_region(int process) {
	interested[process] = FALSE;
}

void* task_without_lock(void* args) {
	int* id = (int*) args;
	
	for (int i = 0; i < 10; i++) {
		int value_counter = counter;
		printf("Thread %d - Counter value: %d\n", *id, value_counter);
		pthread_yield();
		counter = value_counter + 1;
	}
	
	return NULL;
}

void* task_with_lock(void* args) {
	int* id = (int*) args;
	
	for(int i = 0; i < 10; i++) {
		enter_region(*id-1);
		int value_counter = counter;
		printf("Thread %d - Counter value: %d\n", *id, value_counter);
		pthread_yield();
		counter = value_counter + 1;
		leave_region(*id-1);
	}
	
	return NULL;
}

int main() {
	setbuf(stdout, NULL);
	int op;
	printf("Enter option: 1. Without lock; 2. with lock: ");
	scanf("%d", &op);
	
	pthread_t threads[2];
	int ids[2];
	
	for(int i = 0; i < 2; i++) {
		ids[i] = i+1;
		if (op == 1)
			pthread_create(&threads[i], NULL, task_without_lock, (void*)&ids[i]);
		else
			pthread_create(&threads[i], NULL, task_with_lock, (void*)&ids[i]);
	}
	
	for(int i = 0; i < 2; i++) {
		pthread_join(threads[i], NULL);
	}
	
	return 0;
}
