#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdbool.h>
#include <pthread.h>

typedef struct queue_node_t queue_node_t;
typedef struct queue_t queue_t;

struct queue_node_t
{
	queue_node_t* next;
	void* data;
};

struct queue_t
{
	size_t count;
	pthread_mutex_t mutex;
	queue_node_t* first;
	queue_node_t* last;
};

queue_t*
queue_init (/* void */);

bool
queue_is_empty (queue_t* queue);

void 
queue_enqueue(queue_t* queue, void* data);

void
queue_dequeue(queue_t* queue, void** data);

void
queue_destroy(queue_t* queue, void (void*));

#endif