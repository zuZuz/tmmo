#include "query_processing.h"
#include "threadpool.h"

static void query_processing_new(void *message, void *out_queue)
{
    char *data = ((msg_t*)message)->body + sizeof( message_type_t );
    size_t length = ((msg_t*)message)->len - sizeof( message_type_t );

    //TODO: process

    queue_enqueue( (queue_t*)out_queue, message);
    pthread_cond_signal( &((queue_t*)out_queue)->cond );
}

static void query_processing_start(jqueue_t *jqueue, qprocess_args_t *args)
{
    msg_t *message;
    queue_t *in_queue = args -> in;

    while( !*(args->is_terminated) )
    {
        pthread_mutex_lock( &(in_queue->mutex) );


        while(queue_is_empty(in_queue) && !*(args->is_terminated) )
            pthread_cond_wait( &(in_queue->cond), &(in_queue->mutex) );

        queue_dequeue(in_queue, (void*)&message);


        pthread_mutex_unlock( &(in_queue->mutex) );

        if(message != NULL)
            jqueue_add_job(jqueue, query_processing_new, message, args -> out);

    }

}


void* query_processing(void *arg)
{
    threadpool_t *thpool = threadpool_create(THREADS_CNT);

    query_processing_start( threadpool_get_jqueue(thpool), (qprocess_args_t*)arg);

    threadpool_destroy(thpool);

    return NULL;
}


