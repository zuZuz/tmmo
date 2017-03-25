#include <stdio.h>
#include <stdlib.h>

#include <query_processing.h>
#include <job_queue.h>

#include "crypto.h"
#include "network.h"
#include "threads.h"
#include "server.h"

static void* receiver_thread(void* arg)
{
	receiver_args* args = (receiver_args*) arg;
	msg_t* msg = NULL;
	
	jqueue_t* in = args->in;
	queue_t* out = args->out;

	while (true)
	{
		msg = msg_recv(args->con);

		if (!msg)
		{
			continue;
		}

		printf("recv: %s \n", msg->body);
        jqueue_add_job(in, query_processing_new, msg, out);
		msg = NULL;
	}

	return NULL;
}

static void* sender_thread(void *args)
{
	msg_t *msg;
	sender_arg *arg = (sender_arg *) args;

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
		pthread_mutex_unlock(&(arg->queue->mutex));
		queue_dequeue(arg->queue, (void **) &msg);

		if (crypto_key_is_empty(msg->key))
		{
			crypto_gen_key(msg->key, KEY_LEN);
		}

		printf("sent: %s \n", msg->body);
		msg_send(arg->con, msg);
		msg_destroy(msg);
	}

	return NULL;
}

int run_input_thread(pthread_t* tid, jqueue_t* in, queue_t* out, conn_t* con)
{
	receiver_args arg;
	arg.in = in;
	arg.out = out;
	arg.con = con;

	printf("Starting input thread\n");
	return pthread_create(tid, NULL, receiver_thread, &arg);
}


int run_output_thread(pthread_t* tid, queue_t* queue, conn_t* con)
{
	sender_arg arg;
	arg.queue = queue;
	arg.con = con;

	printf("Starting output thread\n");
	return pthread_create(tid, NULL, sender_thread, &arg);
}