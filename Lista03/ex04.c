#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    int finish = 0;
    char command_to_run[2048];

    while (!finish) {
        printf("> ");
        fgets(command_to_run, 2048, stdin);
        command_to_run[strlen(command_to_run)-1] = '\0';

        char* first_value = strtok(command_to_run, " ");

        if (!strcmp(first_value, "cd")) {
            char* path = strtok(NULL, " ");
            if (chdir(path) == -1)
                fprintf(stderr, "Error to change to %s directory\n", path);
        }
        if (!strcmp(first_value, "pwd")) {
            char dir_name[2048];
            getcwd(dir_name, 2048);
            printf("%s\n", dir_name);
        }
        if (!strcmp(first_value, "exit")) finish = 1;
    }

    return 1;
}