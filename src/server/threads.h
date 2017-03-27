#ifndef _THREADS_H_
#define _THREADS_H_

#include "network.h"
#include "queue.h"
#include "player.h"

#include <job_queue.h>

typedef struct receiver_args receiver_args;
typedef struct sender_arg sender_arg;
typedef struct server_arg server_arg;

struct receiver_args
{
	jqueue_t* in;
	conn_t* con;
	queue_t* out;
	bool* is_terminated;
	players_t* players;
};

struct sender_arg
{
	queue_t* queue;
	conn_t* con;
	bool* is_terminated;
};

struct server_arg
{
	jqueue_t* in;
	bool* is_terminated;
	unsigned long timeout;
	players_t* players;
};

int
run_game_thread(pthread_t* game, bool* is_stopped);

void*
receiver_thread(void* args);

void*
sender_thread(void *args);

int 
run_input_thread(pthread_t* tid, jqueue_t* in, queue_t* out, conn_t* con, players_t* players);


int 
run_server_thread(pthread_t* tid, jqueue_t* in, bool* is_stopped, unsigned long timeout, players_t* players);

int 
run_output_thread(pthread_t* tid, queue_t* queue, conn_t* con);

#endif