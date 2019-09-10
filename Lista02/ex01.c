#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void neto() {
    execlp("/bin/ls","ls",NULL);
}

void child() {
    pid_t pid1, pid2;

    pid1 = fork();
    if (pid1 == 0) {
        neto();
    } else {
        pid2 = fork();
        if (pid2 == 0) {
            neto();
        } else {
            while(wait(NULL)>0);
            printf("My PID: %d - Child\n", getpid());
        }
    }
}

void parent() {
    pid_t pid1, pid2;

    pid1 = fork();
    if (pid1 == 0) {
        child();
    } else {
        pid2 = fork();
        if (pid2 == 0) {
            child();
        } else {
            while(wait(NULL)>0);
            printf("My PID: %d - Parent\n", getpid());
        }
    }
}

int main() {

    parent();
    return 0;
}