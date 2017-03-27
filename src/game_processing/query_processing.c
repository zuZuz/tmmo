#include "query_processing.h"
#include "game_functions.h"
#include "shared_funcs.h"

static queue_t* out_queue;

static void query_text(msg_t *msg, msg_t **reply_msg)
{
    char *pos = msg->body;
    char *func_name;

    func_name = get_word(&pos);

    if(func_name != NULL)
    {
        ( gfunc_get(func_name) ) (msg, reply_msg, pos);
    }
}

void query_processing_new(void *message)
{
    bool have_asnw = false;

    msg_t *reply_msg = NULL;

    switch( ((msg_t*)message) -> type )
    {
        case user_msg: query_text( (msg_t*)message, &reply_msg );
            break;

        case conn_test: have_asnw = true;
            break;

        default: return;
    }

    if(!have_asnw)
    {
        have_asnw = reply_msg != NULL;
        msg_destroy((msg_t*)message);
    }
    else
    {
        reply_msg = message;
    }

    if(have_asnw == true)
    {
        queue_enqueue(out_queue, reply_msg);
        pthread_cond_signal(&(out_queue->cond));
    }


}

void query_processing_set_out_queue(queue_t* _out_queue)
{
    out_queue = _out_queue;
}