#ifndef _THREADS_H_
#define _THREADS_H_

#include "network.h"
#include "queue.h"

#include "../game_processing/job_queue.h"

typedef struct receiver_args receiver_args;
typedef struct sender_arg sender_arg;

struct receiver_args
{
	conn_t* con;
	jqueue_t* in;
	queue_t* out;
	bool* is_terminated;
};

struct sender_arg
{
	queue_t* queue;
	conn_t* con;
	bool* is_terminated;
};

void*
receiver_thread(void* args);

void*
sender_thread(void *args);

int 
run_input_thread(pthread_t* tid, jqueue_t* in, queue_t* out, conn_t* con);

int 
run_output_thread(pthread_t* tid, queue_t* queue, conn_t* con);

#endif