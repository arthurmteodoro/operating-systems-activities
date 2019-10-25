#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <sys/sendfile.h>
#include <limits.h>
#include <errno.h>

#define BUF_LEN (1 * (sizeof(struct inotify_event) + NAME_MAX + 1))

void displayInotifyEvent(struct inotify_event *i) {
    printf("    wd =%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);

    printf("mask = ");
    if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE)        printf("IN_CREATE ");
    if (i->mask & IN_DELETE)        printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN)          printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
    printf("\n");

    if (i->len > 0)
        printf("        name = %s\n", i->name);
}

void handler_event(struct inotify_event* event, char* path_watcher, char* dest, mqd_t queue) {
    char path_event[PATH_MAX];
    char path_copy[PATH_MAX];

    char buf[BUF_LEN];

    strcpy(path_event, path_watcher);
    strcat(path_event, event->name);

    strcpy(path_copy, dest);
    strcat(path_copy, event->name);

    printf("event handler: %s\n", path_event);
    printf("path_copy: %s\n", path_copy);

    if (event->mask & IN_CREATE) {
        if (event->mask & IN_ISDIR) {
            mkdir(path_copy, 0644);
        } else {
            int fd = open(path_copy, O_CREAT, 0644);
            close(fd);
        }
    } else if (event->mask & IN_DELETE) {
        if (event->mask & IN_ISDIR) {
            rmdir(path_copy);
        } else {
            remove(path_copy);
        }
    } else if (event->mask & IN_MODIFY) {
        FILE* src = fopen(path_event, "r");
        FILE* dest = fopen(path_copy, "w");

        char ch = fgetc(src);
        while (ch != EOF) {
            fputc(ch, dest);
            ch = fgetc(src);
        }

        fclose(src);
        fclose(dest);
    } else if (event->mask & IN_MOVED_FROM) {
        struct inotify_event *event2;
        char path_copy_2[PATH_MAX];

        mq_receive(queue, buf, BUF_LEN, 0);
        event2 = (struct inotify_event *) buf;
        strcpy(path_copy_2, dest);
        strcat(path_copy_2, event2->name);

        rename(path_copy, path_copy_2);
    }
}

int main(int argc, char* argv[]) {
    char buf[BUF_LEN];

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct inotify_event) + NAME_MAX + 1;
    attr.mq_curmsgs = 0;

    mqd_t mq_copy = mq_open("/filecopy", O_CREAT | O_RDWR, 0644, &attr);

    for(;;) {
        struct inotify_event *event;

        mq_receive(mq_copy, buf, BUF_LEN, 0);

        event = (struct inotify_event *) buf;

        displayInotifyEvent(event);
        handler_event(event, argv[1], argv[2], mq_copy);
    }
}