#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "crypto.h"
#include "network.h"
#include "threads.h"
#include "server.h"

#include <query_processing.h>
#include <job_queue.h>
#include <game_main.h>

void* receiver_thread(void* arg)
{
	msg_t* msg = NULL;
	receiver_args* args = (receiver_args*) arg;
	jqueue_t* in = args->in;
	players_t* players = args->players;
	msg_t* note;

	while (true)
	{
		msg = msg_recv(args->con);

		if (!msg)
		{
			continue;
		}

		printf("recv: %s \n", msg->body);
        jqueue_add_job(in, query_processing_new, msg);

        if (crypto_key_is_empty(msg->key))
        {
    		crypto_gen_key(msg->key, TOKEN_LEN);
        }

        if (!player_update(players, msg->key))
        {
        	player_add(players, msg->key, msg->addr);
        	note = msg_init(NULL);
        	note->addr = msg->addr;
        	msg_set_key(note, msg->key);
        	msg_set_type(note,logged_in);
        	jqueue_add_job(in, query_processing_new, note);
        }
	}

	if (msg) msg_destroy(msg);
	return NULL;
}

int run_input_thread(pthread_t* tid, jqueue_t* in, queue_t* out, conn_t* con, players_t* players)
{
	receiver_args arg;

	arg.in = in;
	arg.out = out;
	arg.con = con;
	arg.players = players;

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


		if (crypto_key_is_empty(msg->key))
		{
			crypto_gen_key(msg->key, TOKEN_LEN);
		}

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

void* server_thread(void* args)
{
	size_t i;
	server_arg* arg = (server_arg*) args;
	bool is_stopped = *(arg->is_terminated);
	players_t* players = arg->players;
	jqueue_t* in = arg->in;
	unsigned long timeout = arg->timeout;
	msg_t* note;

	while (!is_stopped)
	{
		for (i = 0; i < players->count; i++)
		{
			if ((time(NULL) - players->p[i]->timestamp) > timeout)
			{
				note = msg_init(NULL);
				note->addr = players->p[i]->addr;
				msg_set_key(note, players->p[i]->token);
				msg_set_type(note, timed_out);
				player_delete(players, i);
				jqueue_add_job(in, query_processing_new, note);
			}
		}

		usleep(1000000);
	}

	return NULL;
}

int run_server_thread(pthread_t* tid, jqueue_t* in, bool* is_stopped, unsigned long timeout, players_t* players)
{
	server_arg* arg = malloc(sizeof(server_arg));

	arg->in = in;
	arg->is_terminated = is_stopped;
	arg->timeout = timeout;
	arg->players = players;

	return pthread_create(tid, NULL, server_thread, (void*) arg);
}

static void* game_thread(void* args)
{
	bool* is_stopped = (bool*) args;
	game_start(is_stopped);
	
	return NULL;
}

int run_game_thread(pthread_t* tid, bool* is_stopped)
{
	printf("Starting game thread\n");
	return pthread_create(tid, NULL, game_thread, &is_stopped);
}