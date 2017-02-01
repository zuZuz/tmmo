#include "query_processing.h"
#include "game_functions.h"
#include "threadpool.h"
#include "shared_funcs.h"

static bool query_text(msg_t *msg)
{
    char *pos = msg->body;
    char *func_name;

    func_name = get_word(&pos);

    if(func_name != NULL)
    {
        ( gfunc_get(func_name) ) (msg, pos);
        return true;
    }
    else
        return false;
}

static void query_processing_new(void *message, void *out_queue)
{
    bool have_asnw;

    switch( ((msg_t*)message) -> type )
    {
        case text: have_asnw = query_text( (msg_t*)message );
            break;

        case conn_test: have_asnw = true;
            break;

        default: return;
    }

    if(have_asnw == true)
    {
        queue_enqueue((queue_t *) out_queue, message);
        pthread_cond_signal(&((queue_t *) out_queue)->cond);
    }
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


        pthread_mutex_unlock( &(in_queue->mutex) );

        queue_dequeue(in_queue, (void*)&message);

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