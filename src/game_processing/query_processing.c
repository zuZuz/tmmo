#include "query_processing.h"
#include "game_functions.h"
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

void query_processing_new(void *message, void *out_queue)
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
