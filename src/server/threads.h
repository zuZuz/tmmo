#ifndef _THREADS_H_
#define _THREADS_H_

#include "network.h"
#include "queue.h"

typedef struct thread_arg thread_arg;

struct thread_arg
{
	queue_t* queue;
	conn_t* con;
	bool* is_terminated;
};


void*
receiver_thread(void* args);


void*
sender_thread(void *args);

#endif