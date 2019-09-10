#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

unsigned long long circle_points = 0, square_points = 0;
int fd[2];
int stop_monte_carlo = 0;

void calc_pi_monte_carlo() {
    close(fd[READ]);
    srand(time(NULL));

    while (!stop_monte_carlo) {
        double rand_x = drand48();
        double rand_y = drand48();

        double origin_dist = rand_x * rand_x + rand_y * rand_y;
        if (origin_dist <= 1)
            circle_points++;
        square_points++;
    }

    double pi = (4*(double)circle_points) / (double)square_points;
    write(fd[WRITE], &pi, sizeof(double));

    close(fd[WRITE]);
}

void handler_signal(int sig) {
    stop_monte_carlo = 1;
}

void filho() {
    signal(SIGALRM, handler_signal);

    struct itimerval timer;
    timer.it_value.tv_sec = 60;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);
    calc_pi_monte_carlo();
}

int main() {
    pipe(fd);

    pid_t pid;
    pid = fork();
    if (pid == 0) {
        filho();
    } else {
        while(wait(NULL)>0);
        double son_pi;

        read(fd[READ], &son_pi, sizeof(double));
        printf("Pi value: %lf, PID: %d\n", son_pi, getpid());
    }

    return 0;
}
