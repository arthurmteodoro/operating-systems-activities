#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define N 5
#define LEFT (i+N-1)%N
#define RIGHT (i+1)%N

#define THINKING 0
#define HUNGRY 1
#define EATING 2

int state[N] = {THINKING};
sem_t mutex;
sem_t s[N];

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

    while(1) {
        think(*i);
        take_forks(*i);
        eat(*i);
        put_forks(*i);
    }
}

int main() {
    pthread_t thread_phy[N];
    int phy_id[N];

    sem_init(&mutex, 0, 1);

    for(int i = 0; i < N; i++) {
        sem_init(&s[i], 0, 0);
    }

    for(int i = 0; i < N; i++) {
        phy_id[i] = i;
        pthread_create(&thread_phy[i], NULL, philosopher, &phy_id[i]);
    }

    for(int i = 0; i < N; i++) {
        pthread_join(thread_phy[i], NULL);
    }
}