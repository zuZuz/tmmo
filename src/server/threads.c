#include <stdio.h>
#include <stdlib.h>

#include "crypto.h"
#include "network.h"
#include "threads.h"
#include "server.h"

#include "../game_processing/query_processing.h"
#include "../game_processing/job_queue.h"

void* receiver_thread(void* arg)
{
	msg_t* msg = NULL;
	receiver_args* args = (receiver_args*) arg;
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

	if (msg) msg_destroy(msg);
	return NULL;
}

int run_input_thread(pthread_t* tid, jqueue_t* in, queue_t* out, conn_t* con)
{
	receiver_args arg;

	arg.in = in;
	arg.out = out;
	arg.con = con;

	return pthread_create(tid, NULL, receiver_thread, &arg);
}

void* sender_thread(void *args)
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

		printf("sent: %s \n", msg->body);
		msg_send(arg->con, msg);
		msg_destroy(msg);
		msg = NULL;
	}

	if (msg) msg_destroy(msg);
	return NULL;
}

int run_output_thread(pthread_t* tid, queue_t* queue, conn_t* con)
{
	sender_arg arg;

	arg.queue = queue;
	arg.con = con;

	return pthread_create(tid, NULL, sender_thread, &arg);
}