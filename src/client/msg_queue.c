#include "msg_queue.h"

#include <malloc.h>

static void msg_queue_init_empty(msg_queue_t* queue)
{
    queue->count = 0;
    queue->first = NULL;
    queue->last = NULL;
}

msg_queue_t* msg_queue_init()
{
    msg_queue_t* queue = malloc(sizeof(msg_queue_t));
    if (!queue)
    {
        return NULL;
    }

    pthread_mutex_init(&(queue->mutex), NULL);
    pthread_cond_init(&(queue->cond), NULL);
    msg_queue_init_empty(queue);

    return queue;
}

bool msg_queue_is_empty(msg_queue_t* queue)
{
    if (queue->count > 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void msg_queue_add(msg_queue_t* queue, msg_t* data)
{
    msg_queue_node_t* node = malloc(sizeof(msg_queue_node_t));
    if (!node)
    {
        return;
    }

    node->data = data;

    pthread_mutex_lock(&queue->mutex);

    if (!queue_is_empty(queue))
    {
        queue->last->next = node;
        queue->last = node;
    }
    else
    {
        queue->last = node;
        queue->first = queue->last;
    }

    queue->count++;
    pthread_mutex_unlock(&queue->mutex);
    pthread_cond_signal(&queue->cond);
}

void msg_queue_destroy(msg_queue_t* queue)
{
    msg_queue_node_t* node;

    while (queue->count > 0)
    {
        node = queue->first;
        queue->first = node->next;

        msg_destroy(node->data);

        free(node);
        queue->count--;
    }

    pthread_mutex_destroy(&queue->mutex);
    free(queue);
}
