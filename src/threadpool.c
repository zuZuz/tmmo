#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include "threadpool.h"

typedef struct threadpool
{
    pthread_t *threads;
    jqueue_t *jqueue;
    bool threads_active;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    size_t threads_cnt;
} threadpool_t;


static void* thread_function(void *thpool)
{
    job_t *job;

    while( ((threadpool_t*)thpool) -> threads_active )
    {
        pthread_mutex_lock(&( ((threadpool_t*)thpool) -> mutex ));


        while(jqueue_is_empty( ((threadpool_t*)thpool)->jqueue ) && ((threadpool_t*)thpool) -> threads_active)
            pthread_cond_wait(&( ((threadpool_t*)thpool) -> cond ), &( ((threadpool_t*)thpool) -> mutex ));

        job = jqueue_get_job( ((threadpool_t*)thpool) ->jqueue );


        pthread_mutex_unlock(&( ((threadpool_t*)thpool) -> mutex ));

        if(job != NULL)
            do_job( job );
    }

    return NULL;
}

threadpool_t* threadpool_create(size_t threads_cnt)
{
    threadpool_t *thpool = malloc( sizeof(threadpool_t) );

    thpool->threads = malloc( sizeof(pthread_t) * threads_cnt );
    if(thpool->threads == NULL)
        return NULL;

    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init( &(thpool->mutex), &mutex_attr );

    pthread_cond_init( &(thpool->cond), NULL );
    thpool->jqueue = jqueue_create( &(thpool->cond), &(thpool->mutex) );
    thpool->threads_active = true;
    thpool->threads_cnt = threads_cnt;

    for(size_t i = 0; i < threads_cnt; i++)
    {
        pthread_create( &(thpool->threads[i]), NULL, thread_function, thpool);
        usleep(1);
    }

    return thpool;
}

void threadpool_destroy(threadpool_t *thpool)
{
    thpool->threads_active = false;

    for(size_t i = 0; i < thpool->threads_cnt; i++)
        pthread_cond_signal( &(thpool->cond) );

    for(size_t i = 0; i < thpool->threads_cnt; i++)
        pthread_join( (thpool->threads)[i] , NULL );

    jqueue_destroy( &(thpool->jqueue) );
    free( thpool->threads );
    free( thpool );
}

jqueue_t* threadpool_get_jqueue(threadpool_t *thpool)
{
    return thpool != NULL ? thpool->jqueue : NULL;
}