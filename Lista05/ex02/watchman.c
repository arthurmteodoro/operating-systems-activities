#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

static void             /* Display information from inotify_event structure */
displayInotifyEvent(struct inotify_event *i)
{
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

void send_event(struct inotify_event *i, mqd_t queue) {
    if ((i->mask & IN_CLOSE_WRITE) ||
        (i->mask & IN_CREATE) ||
        (i->mask & IN_DELETE) ||
        (i->mask & IN_MODIFY) ||
        (i->mask & IN_MOVED_FROM) ||
        (i->mask & IN_MOVED_TO)) {
        displayInotifyEvent(i);

        mq_send(queue, (char*)i, sizeof(struct inotify_event)+i->len, 0);
    }
}

int main(int argc, char* argv[]) {
    int inotifyFd, wd;
    char buf[BUF_LEN];
    size_t numRead;
    char *p;
    struct inotify_event *event;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct inotify_event) + NAME_MAX + 1;
    attr.mq_curmsgs = 0;

    mqd_t mq_copy = mq_open("/filecopy", O_CREAT | O_RDWR, 0644, &attr);

    inotifyFd = inotify_init();                 /* Create inotify instance */
    if (inotifyFd == -1) {
        perror("Create inotify error");
        exit(EXIT_FAILURE);
    }

    wd = inotify_add_watch(inotifyFd, ".", IN_ALL_EVENTS);
    if (wd == -1) {
        perror("Create watch error");
        exit(EXIT_FAILURE);
    }

    for (;;) {                                  /* Read events forever */
        numRead = read(inotifyFd, buf, BUF_LEN);
        if (numRead == 0) {
            perror("Read 0 value");
            exit(EXIT_FAILURE);
        }

        if (numRead == -1) {
            perror("Read -1 value");
            exit(EXIT_FAILURE);
        }

        for (p = buf; p < buf + numRead; ) {
            event = (struct inotify_event *) p;
            send_event(event, mq_copy);

            p += sizeof(struct inotify_event) + event->len;
        }
    }

    mq_close(mq_copy);
    close(inotifyFd);

    return EXIT_SUCCESS;
}
