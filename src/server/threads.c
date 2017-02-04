#include <stdio.h>
#include <stdlib.h>

#include "crypto.h"
#include "network.h"
#include "threads.h"
#include "server.h"

volatile extern bool is_terminated;

void* receiver_thread(void* args)
{
	msg_t* msg;
	thread_arg* arg = (thread_arg*) args;

	while (true)
	{
		msg = msg_recv(arg->con);

        if (crypto_key_is_empty(msg->key))
        {
            crypto_gen_key(msg->key, KEY_LEN);
        }

        queue_enqueue(arg->queue, msg);
	}

	return NULL;
}

void* sender_thread(void *args)
{
    msg_t *msg;
    thread_arg *arg = (thread_arg *) args;

    while (true)
    {
        pthread_mutex_lock(&(arg->queue->mutex));
        
        while (queue_is_empty(arg->queue))
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

        printf("sent: %s \n", msg->body);
        msg_send(arg->con, msg);
        msg_destroy(msg);
    }

    printf("shutdowning\n");
    return NULL;
}