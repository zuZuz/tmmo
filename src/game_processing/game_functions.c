#include "game_functions.h"
#include "str_hashtable.h"
#include "shared_funcs.h"
#include <string.h>
#include <time.h>

//the number of gaming functions
#define FUNCS_CNT 2

typedef struct func_name
{
    void (*func)(msg_t*, char*);
    char *name;
} func_name_t;


static str_hashtable_t *funcs_hashtable;

static void gfunc_go(msg_t *msg, char *args)
{
    char *answ = get_word(&args);

    msg->len = strlen(answ);
    memcpy(msg->body, answ, msg->len + 1);
}

static void gfunc_hello(msg_t *msg, char *args)
{
    int r = rand();

    char *answ = r % 2 ? "Hi!" : "Hello. How are you?";

    msg->len = strlen(answ);
    memcpy(msg->body, answ, msg->len + 1);
}

static void gfunc_dntknow(msg_t *msg, char *args)
{

    char *answ = "I don't know...";

    msg->len = strlen(answ);
    memcpy(msg->body, answ, msg->len + 1);
}


bool gfunc_init(char **err)
{
    srand(time(NULL));

    funcs_hashtable = str_hashtable_create(FUNCS_CNT);
    func_name_t funcs[FUNCS_CNT] =
            {
                    {gfunc_hello, "hello"},
                    {gfunc_go, "go"},
            };

    for(int i = 0; i < FUNCS_CNT; i++)
    {
        if( !str_hashtable_add(funcs_hashtable, funcs[i].name, funcs[i].func, NULL) )
        {
            *err = funcs[i].name;
            return false;
        }
    }

    return true;
}

void (* gfunc_get(char *func_name) )(msg_t*, char*)
{
    void (*func)(msg_t*, char*);
    func = str_hashtable_get(funcs_hashtable, func_name);

    return (func != NULL) ? func : gfunc_dntknow;
}

void gfunc_destroy()
{
    str_hashtable_destroy(funcs_hashtable);
}