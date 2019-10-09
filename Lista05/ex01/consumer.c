#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <unistd.h>
#include "commom.h"

int main() {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(int);
    attr.mq_curmsgs = 0;

    mqd_t mq_producer = mq_open(QUEUE_NAME_PRODUCER, O_CREAT | O_RDWR, 0644, &attr);
    mqd_t mq_consumer = mq_open(QUEUE_NAME_CONSUMER, O_CREAT | O_RDWR, 0644, &attr);

    for(int i = 0; i < MAX_QUEUE; i++)
        mq_send(mq_producer, (char*) &i, sizeof(int), 0);

    int item;
    for(;;) {
        mq_receive(mq_consumer, (char*) &item, sizeof(int), 0);
        fprintf(stderr, "Consumidor recebeu o item %d\n", item);
        mq_send(mq_producer, (char*) &item, sizeof(int), 0);

        usleep(100*1000);
    }

    mq_close(mq_consumer);
    mq_close(mq_producer);

    mq_unlink(QUEUE_NAME_CONSUMER);
    mq_unlink(QUEUE_NAME_PRODUCER);

    return 0;
}
