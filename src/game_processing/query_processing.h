/*
 * Processing requests from the queue "in" and adding the responses to the queue "out".
 */

#include "../server/queue.h"
#include "../server/network.h"

#ifndef QUERY_PROCESSING_H
#define QUERY_PROCESSING_H

//the number of threads in the thread pool
#define THREADS_CNT 5

typedef struct qprocess_args
{
    queue_t *in;
    queue_t *out;
    bool *is_terminated;
} qprocess_args_t;

void* query_processing(void *arg);

#endif //QUERY_PROCESSING_H