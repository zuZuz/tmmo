#include "game_functions.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

//the number of gaming functions
#define FUNCS_CNT 1

typedef struct func_node
{
    void (*func)(msg_t*, char*);
    char *func_name;
    struct func_node *next;
} func_node_t;


static func_node_t fnodes[FUNCS_CNT] = {NULL};


static void gfunc_hello(msg_t *msg, char *args)
{
    int r = rand();

    char *answ = r % 2 ? "Hi!" : "Hello. How are you?";

    msg->len = strlen(answ);
    memcpy(msg->body, answ, msg->len + 1);
}

static void gfunc_dntknow(msg_t *msg, char *args)
{

    char *answ = "I do not know...";

    msg->len = strlen(answ);
    memcpy(msg->body, answ, msg->len + 1);
}


/*
 * Bob Jenkins's hash function
 */
static uint32_t jenkins_one_at_a_time_hash(char *key, size_t len)
{
    uint32_t hash, i;
    for(hash = i = 0; i < len; ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

static void gfunc_add(void (*func)(msg_t*, char*),  char *func_name)
{
    func_node_t *node = fnodes;
    uint32_t index = jenkins_one_at_a_time_hash(func_name, strlen(func_name)) % FUNCS_CNT;

    node += index;

    if(node->func != NULL)
    {
        while(node->next != NULL)
        {
            node = node->next;
        }

        node->next = malloc( sizeof(func_node_t) );
        node = node->next;
    }

    node->func = func;
    node->func_name = func_name;
    node->next = NULL;

}



void gfunc_init()
{
    srand(time(NULL));
    gfunc_add(gfunc_hello, "hello");
}

void (* gfunc_get(char *func_name) )(msg_t*, char*)
{
    func_node_t *node = fnodes;
    uint32_t index = jenkins_one_at_a_time_hash(func_name, strlen(func_name)) % FUNCS_CNT;

    node += index;

    do
    {
        if (strcmp(node->func_name, func_name) == 0)
            return node->func;

        node = node->next;

    } while(node != NULL);

    return gfunc_dntknow;
}