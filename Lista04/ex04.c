#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define LEFT (i+size-1)%size
#define RIGHT (i+1)%size

#define THINKING 0
#define HUNGRY 1
#define EATING 2

int size = 0;
int* state;
sem_t mutex;
sem_t* s;

void test(int i) {
    if(state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[i] = EATING;
        sem_post(&s[i]);
    }
}

void take_forks(int i) {
    sem_wait(&mutex);
    state[i] = HUNGRY;
    test(i);
    sem_post(&mutex);
    sem_wait(&s[i]);
}

void put_forks(int i) {
    sem_wait(&mutex);
    state[i] = THINKING;
    test(LEFT);
    test(RIGHT);
    sem_post(&mutex);
}

void think(int i) {
    fprintf(stderr, "Philosopher %d thinking\n", i);
    int value = rand() % 500;
    usleep(value * 1000);
}

void eat(int i) {
    fprintf(stderr, "Philosopher %d eating\n", i);
    int value = rand() % 500;
    usleep(value * 1000);
}

void* philosopher(void* args) {
    int* i = (int*) args;

    for(int j = 0; j < 5; j++) {
        think(*i);
        take_forks(*i);
        eat(*i);
        put_forks(*i);
    }

    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: main <number_of_phy>");
        exit(EXIT_FAILURE);
    }

    size = (int) strtol(argv[1], NULL, 10);

    state = (int*) malloc(sizeof(int)*size);
    s = (sem_t*) malloc(sizeof(sem_t)*size);

    pthread_t* thread_phy;
    int* phy_id;

    thread_phy = (pthread_t*) malloc(sizeof(pthread_t)*size);
    phy_id = (int*) malloc(sizeof(int)*size);

    sem_init(&mutex, 0, 1);

    for(int i = 0; i < size; i++) {
        sem_init(&s[i], 0, 0);
    }

    for(int i = 0; i < size; i++) {
        phy_id[i] = i;
        pthread_create(&thread_phy[i], NULL, philosopher, &phy_id[i]);
    }

    for(int i = 0; i < size; i++) {
        pthread_join(thread_phy[i], NULL);
    }

    sem_destroy(&mutex);
    for(int i = 0; i < size; i++) {
        sem_destroy(&s[i]);
    }
    free(state);
    free(s);
    free(thread_phy);
    free(phy_id);

    return 0;
}