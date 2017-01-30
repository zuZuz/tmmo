/*
 * Thread pool using the job queue.
 */

#include <stdlib.h>
#include "job_queue.h"

#ifndef THREADPOOL_H
#define THREADPOOL_H

typedef struct threadpool threadpool_t;

threadpool_t* threadpool_create(size_t threads_cnt);
void threadpool_destroy(threadpool_t *thpool);
jqueue_t* threadpool_get_jqueue(threadpool_t *thpool);

#endif //THREADPOOL_H