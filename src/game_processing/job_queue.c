#include <stdlib.h>
#include "job_queue.h"

typedef struct jqueue
{
    struct job_queue_node *first_node;
    struct job_queue_node *last_node;
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
} jqueue_t;

typedef struct job_queue_node
{
    struct job_queue_node *next;
    struct job *qjob;
} job_queue_node_t;

typedef struct job
{
    void (*function)(void*);
    void *arg1;
} job_t;


jqueue_t* jqueue_create(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    jqueue_t* head = malloc( sizeof(jqueue_t) );

    head->first_node = NULL;
    head->last_node = (job_queue_node_t*)head;
    head->cond = cond;
    head->mutex = mutex;
    return head;
}

void jqueue_add_job(jqueue_t* head, void (*function)(void*), void *arg1)
{
    job_queue_node_t* new_node = malloc( sizeof(job_queue_node_t) );

    new_node->next = NULL;
    new_node->qjob = malloc( sizeof(job_t) );
    new_node->qjob->arg1 = arg1;
    new_node->qjob->function = function;

    pthread_mutex_lock(head->mutex);

    (head->last_node)->next = new_node;
    head->last_node = new_node;

    pthread_mutex_unlock(head->mutex);
    pthread_cond_signal(head->cond);
}

job_t* jqueue_get_job(jqueue_t* head)
{
    pthread_mutex_lock(head->mutex);

    job_queue_node_t* first = head->first_node;
    if(first == NULL)
    {
        pthread_mutex_unlock(head->mutex);
        return NULL;
    }

    job_t* ret_job = first->qjob;

    head->first_node = first->next;
    if(head->last_node == first)
        head->last_node = (job_queue_node_t*)head;

    free(first);

    pthread_mutex_unlock(head->mutex);
    return ret_job;
}

bool jqueue_is_empty(jqueue_t* head)
{
    return (head->first_node == NULL);
}

void jqueue_destroy(jqueue_t** head)
{
    job_queue_node_t* current_node = (*head)->first_node;

    pthread_mutex_lock((*head)->mutex);

    while(current_node != NULL)
    {
        free(current_node->qjob);
        (*head)->first_node = current_node->next;
        free(current_node);
        current_node = (*head)->first_node;
    }

    pthread_mutex_unlock((*head)->mutex);

    free(*head);
    *head = NULL;
}

void do_job(job_t* job)
{
    job->function(job->arg1);
}