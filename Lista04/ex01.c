#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

#define BUF_SIZE 10

void producer(int* buf, int* value) {
    sem_t* empty = sem_open("/sem_empty", O_CREAT, 0644, BUF_SIZE);
    sem_t* full = sem_open("/sem_full", O_CREAT, 0644, 0);
    sem_t* mutex = sem_open("/sem_mutex", O_CREAT, 0644, 1);

    int item = 0;

     for(;;) {
        sem_wait(empty);
        sem_wait(mutex);
        
        int n;
        sem_getvalue(full, &n);

		buf[n] = *value;
        fprintf(stderr, "Produtor fez item %d\n", *value);
        *value = *value + 1;

        sem_post(mutex);
        sem_post(full);

        usleep(100*1000);
    }
    
    sem_close(empty);
    sem_close(full);
    sem_close(mutex);
    
    exit(0);
}
void consumer(int* buf, int* value, int qtd_producers) {
    sem_t* empty = sem_open("/sem_empty", O_CREAT, 0644, BUF_SIZE);
    sem_t* full = sem_open("/sem_full", O_CREAT, 0644, 0);
    sem_t* mutex = sem_open("/sem_mutex", O_CREAT, 0644, 1);

    for(;;) {
        sem_wait(full);
        sem_wait(mutex);

		int n;
        sem_getvalue(full, &n);
        int item = buf[n];
        fprintf(stderr, "Consumidor consumiu: %d\n", item);

        sem_post(mutex);
        sem_post(empty);

        usleep(300*1000);
    }
    
    sem_close(empty);
    sem_close(full);
    sem_close(mutex);
    
    exit(0);
}


int main(int argc, char** argv) {
	if (argc != 3)
		return -1;
		
	int number_procuder = atoi(argv[1]);
	int number_consumer = atoi(argv[2]);
	
    int* buf = mmap(NULL, sizeof(int)*(BUF_SIZE), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int* value = mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *value = 1;

    sem_t* empty = sem_open("/empty_sem", O_CREAT, 0644, BUF_SIZE);
    sem_t* full = sem_open("/full_sem", O_CREAT, 0644, 0);
    sem_t* mutex = sem_open("/mutex_sem", O_CREAT, 0644, 1);

	for(int i = 0; i < number_procuder; i++) {
		pid_t pid = fork();
		if (pid == 0)
			producer(buf, value);
	}
	for(int i = 0; i < number_consumer; i++) {
		pid_t pid = fork();
		if (pid == 0)
			consumer(buf, value, number_procuder);
	}
	while(wait(NULL) > 0);

    sem_close(empty);
    sem_close(full);
    sem_close(mutex);

    sem_unlink("/empty_sem");
    sem_unlink("/full_sem");
    sem_unlink("/mutex_sem");

    return 0;
}
