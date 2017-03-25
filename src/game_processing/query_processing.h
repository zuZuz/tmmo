/*
 * Processing requests with adding the responses to the queue "out".
 */

#include "../server/queue.h"
#include "../server/network.h"

#ifndef QUERY_PROCESSING_H
#define QUERY_PROCESSING_H

void query_processing_new(void *message, void *out_queue);

#endif //QUERY_PROCESSING_H