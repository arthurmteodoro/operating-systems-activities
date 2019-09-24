#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define BUF_SIZE 2048

char* trim_whitespace(char *str) {
	if (str == NULL) return NULL;
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';
    return str;
}

int count_whitespace(char* str) {
	int counter_wp = 0;
	
	for(int i = 0; i < strlen(str); i++) {
		if (isspace(str[i])) counter_wp++;
	}
	return counter_wp;
}

char** create_args_array(char* src) {
	int number_of_params = count_whitespace(src);
	char** args = (char**) malloc(sizeof(char*)*(number_of_params+2));
	int counter_args = 0;
	
	char* arg = strtok(src, " ");
	while (arg != NULL) {
		args[counter_args++] = arg;
		arg = strtok(NULL, " ");
	}
	args[number_of_params+1] = NULL;
	return args;
}

void executeOneCommand(char** args) {
    pid_t pid = fork();

    if (pid == -1) {
        printf("\nFork error");
        return;
    } else if (pid == 0) {
        char command_base[] = "/bin/";
        char* path_to_bin = strcat(command_base, args[0]);
        if (execve(path_to_bin, args, 0) < 0) {
            printf("Command not found in %s\n", path_to_bin);
        }
        exit(0);
    } else {
        // waiting for child to terminate
        wait(NULL);
        return;
    }
}

void executeTwoCommand(char** args1, char** args2)
{
    // 0 is read end, 1 is write end
    int pipefd[2];
    pid_t pid_1, pid_2;

    if (pipe(pipefd) < 0) {
        printf("Pipe error\n");
        return;
    }
    pid_1 = fork();
    if (pid_1 < 0) {
        printf("Fork error\n");
        return;
    }

    if (pid_1 == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        char command_base_command_1[] = "/bin/";
        char* path_to_bin_command_1 = strcat(command_base_command_1, args1[0]);
        if (execve(path_to_bin_command_1, args1, 0) < 0) {
            printf("Command not found in %s\n", path_to_bin_command_1);
            exit(0);
        }
    } else {
        // Parent executing
        pid_2 = fork();

        if (pid_2 < 0) {
            printf("Fork error");
            return;
        }

        if (pid_2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);

            char command_base_command_2[] = "/bin/";
            char* path_to_bin_command_2 = strcat(command_base_command_2, args2[0]);
            if (execve(path_to_bin_command_2, args2, 0) < 0) {
                printf("Command not found in %s\n", path_to_bin_command_2);
                exit(0);
            }
        } else {
            while (wait(NULL) < 0);
        }
    }
}

int main() {
    setbuf(stdout, NULL);
    char command_to_run[BUF_SIZE];

    while (1) {
        printf("> ");
        fgets(command_to_run, BUF_SIZE, stdin);
        command_to_run[strlen(command_to_run)-1] = '\0';
        char command_backup[BUF_SIZE];
        strcpy(command_backup, command_to_run);

        char* first_value = strtok(command_to_run, " ");
        
        if (first_value != NULL) {
            if (!strcmp(first_value, "cd")) {
                char* path = strtok(NULL, " ");
                if (chdir(path) == -1) fprintf(stderr, "Error to change to %s directory\n", path);
                continue;
            }
            if (!strcmp(first_value, "mkdir")) {
                char* path = strtok(NULL, "");
                struct stat st = {0};
                if (stat(path, &st) == -1){
                    if (mkdir(path, 0700) == -1) fprintf(stderr, "Error to create %s dir\n", path);
                } else fprintf(stderr, "Error to create %s dir - existing dir\n", path);
                continue;
            }
            if (!strcmp(first_value, "rmdir")) {
                char* path = strtok(NULL, "");
                struct stat st = {0};
                if (stat(path, &st) != -1){
                    if (rmdir(path) == -1) fprintf(stderr, "Error to remove %s dir\n", path);
                } else fprintf(stderr, "Error to remove %s dir - dir doest exist\n", path);
                continue;
            }
            if (!strcmp(first_value, "mv")) {
                char* path_source = strtok(NULL, " ");
                char* path_dest = strtok(NULL, "");
                struct stat st = {0};
                if (stat(path_source, &st) != -1){
                    if (rename(path_source, path_dest) == -1) fprintf(stderr, "Error to move %s file to %s \n", path_source, path_dest);
                } else fprintf(stderr, "Error to move %s file - file doest exist\n", path_source);
                continue;
            }
            if (!strcmp(first_value, "pwd")) {
                char dir_name[BUF_SIZE];
                getcwd(dir_name, BUF_SIZE);
                printf("%s\n", dir_name);
                continue;
            }
            if (!strcmp(first_value, "clear")) {
                if (system("CLS")) system("clear");
                continue;
            }
            if (!strcmp(first_value, "exit")) break;

            char* first_command = strtok(command_backup, "|");
            first_command = trim_whitespace(first_command);
            char* second_command = strtok(NULL, "");
            second_command = trim_whitespace(second_command);

            char** args1 = NULL;
            char** args2 = NULL;

            args1 = create_args_array(first_command);
            if (second_command != NULL)
                args2 = create_args_array(second_command);

            if (args2 == NULL)
                executeOneCommand(args1);
            else
                executeTwoCommand(args1, args2);
        }
    }

    return 1;
}
