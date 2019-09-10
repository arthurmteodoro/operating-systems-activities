#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

void swap(int* a, int*b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int check_all_negative(const int* vec, int size) {
	for(int i = 0; i < size; i++)
		if (vec[i] != -1) return 0;
	return 1;
}

int get_first_one_index(const int* vec, int size) {
	for (int i = 0; i < size; i++)
		if (vec[i] == 1) return i;
	return -1;
}

int get_index_lower_available_number(const int* vec, const int* aval, int size) {
	int lower = INT_MAX;
	int index = -1;
	for (int i = 0; i < size; i++)
		if (aval[i] >= 0) {
			if (vec[i] < lower) {
				lower = vec[i];
				index = i;
			}
		}
	return index;
}

// funcao do neto que envia um numero aleatorio para o filho
void neto(int* fd) {
    close(fd[READ]);

    srand(time(NULL) ^ (getpid()<<16));
    int rand_number = rand() % 101;

    write(fd[WRITE], &rand_number, sizeof(int));

    close(fd[WRITE]);
    exit(0);
}

// funcao do filho
void filho(int* fd) {
    close(fd[READ]);
    pid_t pid;
    int fd_filhos[3][2];

    // cria os pipes para comunicar com os netos
    for(int i = 0; i < 3; i++)
        pipe(fd_filhos[i]);

    // cria os 3 netos e passa para eles o pipe q devem usar
    for(int i = 0; i < 3; i++) {
        pid = fork();
        if (pid == 0) {
            // tipo = NETO;
            neto(fd_filhos[i]);
            close(fd_filhos[i][WRITE]);
        }
    }

    // espera todos os filhos terem acabado
    while(wait(NULL)>0);

    // le os valores de cada filho e faz um vetor de 3 posicoes
    int vetor[3];
    for(int i = 0; i < 3; i++) {
        read(fd_filhos[i][READ], &vetor[i], sizeof(int));
        close(fd_filhos[i][READ]);
    }

    // ordena o vetor com os numeros dos filhos e envia para o pai
    if (vetor[0] > vetor[1]) swap(&vetor[0], &vetor[1]);
    if (vetor[1] > vetor[2]) swap(&vetor[1], &vetor[2]);
    if (vetor[0] > vetor[1]) swap(&vetor[0], &vetor[1]);
    write(fd[WRITE], vetor, sizeof(int)*3);

    close(fd[WRITE]);
    exit(0);
}

void pai() {
    pid_t pid;
    int fd[3][2];

    // cria os pipes para comunicar com os filhos
    for(int i = 0; i < 3; i++)
        pipe(fd[i]);

    // cria os filhos e passa o pipe que ele deve usar
    for(int i = 0; i < 3; i++) {
        pid = fork();
        if (pid == 0) {
            // tipo = FILHO;
            filho(fd[i]);
            close(fd[i][WRITE]);
        }
    }

    // aguarda os filhos terminarem
    while(wait(NULL)>0);

    // cria um vetor com todas as posicoes necessarias e le do pipe de cada um dos filhos
    int finish = 0;
    int pipe_outputs[3];
    int pipe_output_available[3] = {3, 3, 3};
    int pipe_request_read[3] = {1, 1, 1};

    while(!finish) {
        int index_read = get_first_one_index(pipe_request_read, 3);
        while(index_read != -1) {
            read(fd[index_read][READ], &pipe_outputs[index_read], sizeof(int));
            pipe_output_available[index_read]--;
            pipe_request_read[index_read] = 0;

            index_read = get_first_one_index(pipe_request_read, 3);
        }

        int lower_index = get_index_lower_available_number(pipe_outputs, pipe_output_available, 3);
        fprintf(stderr, "%d ", pipe_outputs[lower_index]);
        if (pipe_output_available[lower_index])
            pipe_request_read[lower_index] = 1;
        else
            pipe_output_available[lower_index] = -1;

        finish = check_all_negative(pipe_output_available, 3);
    }

    // fecha o pipe
    for(int i = 0; i < 3; i++) {
        close(fd[i][READ]);
    }
}

int main() {
    pai();
    return 0;
}
