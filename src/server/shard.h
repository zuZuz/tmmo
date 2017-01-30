#ifndef _SHARD_ROUTINES_H_
#define _SHARD_ROUTINES_H_


#include <arpa/inet.h>
#include "queue.h"

typedef struct master_thread_arg master_thread_arg;
typedef struct slave_thread_arg slave_thread_arg;

struct master_thread_arg
{
	queue_t* incoming;

	bool is_terminated;
};

struct slave_thread_arg
{
};

void* 
shard_master_thread(void* args);

void* 
shard_slave_thread(void* args);

#endif