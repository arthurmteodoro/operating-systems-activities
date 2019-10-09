#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define BUF_SIZE 10
#define MAX 20

int buf_index = -1;
int buffer[BUF_SIZE];

pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* producer(void* args) {
    int value = 1;
    for(int i = 0; i < MAX; i++) {
        pthread_mutex_lock(&mutex);

        if(buf_index == BUF_SIZE)
            pthread_cond_wait(&full, &mutex);

        buffer[buf_index++] = value++;
        printf("Produce: %d \n",value-1);

        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);

        usleep(100*1000);
    }

    pthread_exit(0);
}

void* consumer(void* args) {
    for(int i = 0; i < MAX; i++) {
        pthread_mutex_lock(&mutex);

        if(buf_index == -1)
            pthread_cond_wait(&empty, &mutex);

        buf_index -= 1;
        printf("Consume: %d \n",buffer[buf_index]);

        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);

        usleep(300*1000);
    }

    pthread_exit(0);
}

int main()
{
    pthread_t thread_producer, thread_consumer;

    pthread_create(&thread_producer,NULL,producer,NULL);
    pthread_create(&thread_consumer,NULL,consumer,NULL);

    pthread_join(thread_producer,NULL);
    pthread_join(thread_consumer,NULL);


    return 0;
}