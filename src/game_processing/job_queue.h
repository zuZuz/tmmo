/*
 * Thread-safe job queue with the signal when job added.
*/

#include <stdbool.h>
#include <pthread.h>

#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

typedef struct job job_t;
typedef struct jqueue jqueue_t;

/* args:
 *      cond - condition variable for notification
 *      mutex - to lock add/get a job or waiting for the first job
 *
 * return a head of the queue
 */
jqueue_t* jqueue_create(pthread_cond_t *cond, pthread_mutex_t *mutex);


void jqueue_add_job(jqueue_t* head, void (*function)(void*), void *arg1);
job_t* jqueue_get_job(jqueue_t* head);
bool jqueue_is_empty(jqueue_t* head);
void jqueue_destroy(jqueue_t** head);
void do_job(job_t* job);

#endif //JOB_QUEUE_H