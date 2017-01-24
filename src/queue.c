#include <malloc.h>

#include "queue.h"

/*
 *
 *	Thread-safe (??) implementation of queue
 *	using mutex. Need refactoring.
 *  Check queue_enqueue for details.
 *
 */

static void queue_init_empty(queue_t* queue)
{
	queue->count = 0;
	queue->first = NULL;
	queue->last = NULL;
}

queue_t* queue_init()
{
	queue_t* queue = malloc(sizeof(queue_t));
	if (!queue)
	{
		return NULL;
	}

	pthread_mutex_init(&(queue->mutex), NULL);

	queue_init_empty(queue);
	return queue;
}

bool queue_is_empty(queue_t* queue)
{
	if (queue->count > 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void queue_enqueue(queue_t* queue, void* data)
{
	/*
	 * Too many mallocs
	 * Rewrite queue on dynamic array of pointers
	 */

	queue_node_t* node = malloc(sizeof(queue_node_t));
	if (!node)
	{
		return;
	}

	node->data = data;

	pthread_mutex_lock(&queue->mutex);
	
	if (!queue_is_empty(queue))
	{
		queue->last->next = node;
		queue->last = node;
	}
	else
	{
		queue->last = node;
		queue->first = queue->last;
	}

	queue->count++;
	pthread_mutex_unlock(&queue->mutex);
}

void queue_dequeue(queue_t* queue, void** data)
{
	queue_node_t* node = queue->first;
	
	if (queue_is_empty(queue))
	{
		*data = NULL;
		return;
	}

	pthread_mutex_lock(&queue->mutex);
	*data = node->data;
	queue->first = node->next;

	queue->count--;
	free(node);
	pthread_mutex_unlock(&queue->mutex);
}

void queue_destroy(queue_t* queue, void callback(void* data))
{
	queue_node_t* node;

	while (queue->count > 0)
	{
		node = queue->first;
		queue->first = node->next;

		if (callback)
		{
			callback(node->data);
		}

		free(node);
		queue->count--;
	}

	pthread_mutex_destroy(&queue->mutex);
	free(queue);
}
