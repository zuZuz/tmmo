#include <memory.h>
#include "query_processing.h"
#include "game_functions.h"
#include "shared_funcs.h"
#include "character.h"
#include "game_main.h"

static queue_t* out_queue;

static void query_text(msg_t *msg, msg_t **reply_msg)
{
    char *pos = msg->body;
    char *func_name;

    if(strlen(character_get_by_addr( &( msg->addr ) )->name) == 0)
    {
        strcpy(character_get_by_addr( &( msg->addr ) )->name, msg->body);
        character_get_by_addr( &( msg->addr ) )->aggression = true;
        func_name = "hello";
    }
    else
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
    struct sockaddr_in *addr;

    printf("player %s:%d\n", inet_ntoa(((msg_t*)message)->addr.sin_addr), ntohs(((msg_t*)message)->addr.sin_port));

    switch( ((msg_t*)message) -> type )
    {
        case user_msg: query_text( (msg_t*)message, &reply_msg );
            break;

        case conn_test: have_asnw = true;
            break;

        case logged_in:
            addr = malloc( sizeof(struct sockaddr_in) );
            *addr = ((msg_t*)message)->addr;
            character_add(game_get_characters(), character_new(76, 134, "", human, 1, 0, true, addr));

            reply_msg = msg_init(NULL);
            (reply_msg)->addr = ((msg_t*)message)->addr;
            strcpy((reply_msg)->key, ((msg_t*)message)->key);

            (reply_msg)->type = main_msg;
            (reply_msg)->len = (size_t)sprintf((reply_msg)->body, "What is your name?") + 1;

            break;

        case timed_out:
            character_remove(game_get_characters(), character_get_by_addr( &( ((msg_t*)message)->addr ) ));
            break;

        case map_update:
            gfunc_map((msg_t*)message, &reply_msg, NULL);
            break;

        case char_info:
            gfunc_getinfo((msg_t*)message, &reply_msg, NULL);
            break;

        default:
            return;
    }

    if(!have_asnw)
    {
        have_asnw = reply_msg != NULL;
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