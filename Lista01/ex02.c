#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct process {
    char* name;
    struct process* next;
};

typedef struct process* Process;

struct user {
    char* name;
    int uid;
    Process list_process;
    struct user* next;
};

typedef struct user* User;

User add_user_list(User list, int uid, char* name) {
    User new_user = (User) malloc(sizeof(struct user));

    new_user->list_process = NULL;
    new_user->next = NULL;
    new_user->uid = uid;
    new_user->name = (char*) malloc(sizeof(char)*strlen(name)+1);
    strcpy(new_user->name, name);

    if (list == NULL) {
        return new_user;
    }

    User insert_in_front = list;
    User prev = NULL;

    while (insert_in_front != NULL && insert_in_front->uid < uid) {
        prev = insert_in_front;
        insert_in_front = insert_in_front->next;
    }

    if (prev == NULL) {
        new_user->next = insert_in_front;
        return new_user;
    }
    prev->next = new_user;
    new_user->next = insert_in_front;

    return list;
}

Process add_process_list(Process list, char* name) {
    Process new_process = (Process) malloc(sizeof(struct process));

    new_process->name = (char*) malloc(sizeof(char)*strlen(name)+1);
    strcpy(new_process->name, name);
    new_process->next = NULL;

    if (list == NULL) {
        return new_process;
    }

    Process last = list;
    while (last->next != NULL) {
        last = last->next;
    }

    last->next = new_process;
    return list;
}

void add_process_in_user(char* process_name, int uid, User users) {
    User this_user = users;
    while(this_user->uid != uid && this_user->next != NULL) {
        this_user = this_user->next;
    }
    this_user->list_process = add_process_list(this_user->list_process, process_name);
}

void print_process_list(Process list) {
    Process node = list;
    while (node != NULL) {
        printf("%s\n", node->name);
        node = node->next;
    }
}

void print_user_list(User list) {
    User node = list;
    while (node != NULL) {
        printf("%s %d\n", node->name, node->uid);
        print_process_list(node->list_process);
        node = node->next;
    }
}

Process destroy_process_list(Process processes) {
    Process node = processes;
    while (node != NULL) {
        Process remove = node;
        node = node->next;

        free(remove->name);
        free(remove);
    }
    return NULL;
}

User destroy_users_list(User users) {
    User node = users;
    while (node != NULL) {
        User remove = node;
        node = node->next;

        remove->list_process = destroy_process_list(remove->list_process);
        free(remove->name);
        free(remove);
    }
    return NULL;
}

int main() {
    User users = NULL;

    int number_users;
    scanf("%d", &number_users);

    for(int i = 0; i < number_users; i++) {
        int uid;
        char name[32];
        scanf("%s %d", name, &uid);
        users = add_user_list(users, uid, name);
    }

    int number_process;
    scanf("%d", &number_process);
    for(int i = 0; i < number_process; i++) {
        int uid;
        char name[32];
        scanf("%s %d", name, &uid);
        add_process_in_user(name, uid, users);
    }

    print_user_list(users);

    destroy_users_list(users);
    return 0;
}

