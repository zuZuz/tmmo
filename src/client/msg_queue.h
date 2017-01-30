#ifndef _MSG_QUEUE_H
#define _MSG_QUEUE_H

#include "network.h"

#include <stdbool.h>
#include <pthread.h>

typedef struct msg_queue_node_t msg_queue_node_t;
typedef struct msg_queue_t msg_queue_t;

struct msg_queue_node_t
{
    msg_queue_node_t* next;
    msg_t* data;
};

struct msg_queue_t
{
    size_t count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    msg_queue_node_t* first;
    msg_queue_node_t* last;
};

msg_queue_t* msg_queue_init();

bool msg_queue_is_empty(msg_queue_t* queue);

msg_t* msg_queue_dequeue(msg_queue_t* queue);

void msg_queue_enqueue(msg_queue_t* queue, msg_t* msg);
void msg_queue_destroy(msg_queue_t* queue);

#endif
