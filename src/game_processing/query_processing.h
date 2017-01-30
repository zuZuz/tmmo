#include "queue.h"
#include "network.h"

#ifndef QUERY_PROCESSING_H
#define QUERY_PROCESSING_H

#define THREADS_CNT 5

typedef enum message_type
{
    text = 0

} message_type_t;

typedef struct qprocess_args
{
    queue_t *in;
    queue_t *out;
    bool *is_terminated;
} qprocess_args_t;

void* query_processing(void *arg);

#endif //QUERY_PROCESSING_H