#include <pthread.h>

#include "shard.h"
#include "network.h"

void* shard_master_thread(void* args)
{
	master_thread_arg* arg;
	arg  = (master_thread_arg*) args;

	while (!arg->is_terminated)
	{
	}

	return NULL;
}

void* shard_slave_thread(void* args)
{
	slave_thread_arg* arg;
	arg = (slave_thread_arg*) args;


	
	return NULL;
}