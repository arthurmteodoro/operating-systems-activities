#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

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
            if (chdir(path) == -1) fprintf(stderr, "Error to change to %s directory\n", path);
        }
        if (!strcmp(first_value, "mkdir")) {
            char* path = strtok(NULL, "");
            struct stat st = {0};
            if (stat(path, &st) == -1){
                if (mkdir(path, 0700) == -1) fprintf(stderr, "Error to create %s dir\n", path);
            } else fprintf(stderr, "Error to create %s dir - existing dir\n", path);
        }
        if (!strcmp(first_value, "rmdir")) {
            char* path = strtok(NULL, "");
            struct stat st = {0};
            if (stat(path, &st) != -1){
                if (rmdir(path) == -1) fprintf(stderr, "Error to remove %s dir\n", path);
            } else fprintf(stderr, "Error to remove %s dir - dir doest exist\n", path);
        }
        if (!strcmp(first_value, "mv")) {
            char* path_source = strtok(NULL, " ");
            char* path_dest = strtok(NULL, "");
            struct stat st = {0};
            if (stat(path_source, &st) != -1){
                if (rename(path_source, path_dest) == -1) fprintf(stderr, "Error to move %s file to %s \n", path_source, path_dest);
            } else fprintf(stderr, "Error to move %s file - file doest exist\n", path_source);
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