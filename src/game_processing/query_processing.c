#include "query_processing.h"
#include "game_functions.h"
#include "shared_funcs.h"

static queue_t* out_queue;

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

void query_processing_new(void *message)
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
        queue_enqueue(out_queue, message);
        pthread_cond_signal(&(out_queue->cond));
    }
}

void set_out_queue(queue_t* _out_queue)
{
    out_queue = _out_queue;
}