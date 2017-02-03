#include <stdio.h>
#include <stdlib.h>

#include "network.h"
#include "threads.h"
#include "server.h"

void* receiver_thread(void* args)
{
    size_t i = 1;
	msg_t* msg;
	thread_arg* arg = (thread_arg*) args;

	while (!*(arg->is_terminated))
	{
		msg = msg_recv(arg->con);
        printf("%lu) recv: %s \n", i, msg->body);
        queue_enqueue(arg->queue, msg);
        i++;
	}

	return NULL;
}

void* sender_thread(void *args)
{
    size_t i = 1;
    msg_t *msg;
    thread_arg *arg = (thread_arg *) args;

    while (!*(arg->is_terminated))
    {
        pthread_mutex_lock(&(arg->queue->mutex));
        
        while (queue_is_empty(arg->queue) && !*(arg->is_terminated))
        {
            pthread_cond_wait(
                &(arg->queue->cond), 
                &(arg->queue->mutex)
            );
        }

        queue_dequeue(arg->queue, (void **) &msg);
        pthread_mutex_unlock(&(arg->queue->mutex));

        if (!msg) 
        {
            continue;
        }

        printf("%lu) sent: %s \n", i, msg->body);
        msg_send(arg->con, msg);
        msg_destroy(msg);
        i++;
    }

    return NULL;
}