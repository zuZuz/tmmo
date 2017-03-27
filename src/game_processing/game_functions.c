#include "game_functions.h"
#include "str_hashtable.h"
#include "shared_funcs.h"
#include "character.h"
#include "game_main.h"
#include "query_processing.h"
#include <string.h>
#include <time.h>

//the number of gaming functions
#define FUNCS_CNT 4

typedef struct func_name
{
    void (*func)(msg_t*, msg_t**, char*);
    char *name;
} func_name_t;


static str_hashtable_t *funcs_hashtable;

static void gfunc_say(msg_t *msg, msg_t **reply_msg, char *args)
{
    character_t *player;
    player = character_get_by_addr(&(msg->addr));

    *reply_msg = msg_init(NULL);
    (*reply_msg)->addr = msg->addr;
    strcpy((*reply_msg)->key, msg->key);

    (*reply_msg)->len = (size_t)sprintf((*reply_msg)->body, "%s: %s", player->name, args) + 1;
    (*reply_msg)->type = chat_msg;

    for(int i = 0; i < game_get_characters()->count; i++)
    {
        if(!(game_get_characters()->arr[i]->is_player))
            continue;

        msg_t *message;

        message = msg_init(NULL);
        message->addr = *(game_get_characters()->arr[i]->addr);

        message->len = (size_t)sprintf(message->body, "%s: %s", player->name, args) + 1;
        message->type = chat_msg;

        query_processing_new(message);
    }

}

void gfunc_getinfo(msg_t *msg, msg_t **reply_msg, char *args)
{
    character_t *player;
    player = character_get_by_addr(&(msg->addr));

    *reply_msg = msg_init(NULL);
    (*reply_msg)->addr = msg->addr;
    strcpy((*reply_msg)->key, msg->key);

    char_info_t info;
    sprintf((char*)&(info.hp), "%d", player->characteristics.hp);
    sprintf((char*)&(info.arm), "%d", player->characteristics.armor);
    sprintf((char*)&(info.atp), "%d", player->characteristics.power);
    sprintf((char*)&(info.evn), "%d", player->characteristics.evasion);
    sprintf((char*)&(info.nick), "%s", player->name);
    sprintf((char*)&(info.lvl), "%d", player->lvl);

    (*reply_msg)->len = sizeof(char_info_t);
    memcpy((*reply_msg)->body, &info, (*reply_msg)->len);
    (*reply_msg)->type = char_info;

}

void gfunc_map(msg_t *msg, msg_t **reply_msg, char *args)
{
    character_t *player;
    map_point_t *map = game_get_map();
    size_t msize_x = game_get_msize_x();

    player = character_get_by_addr(&(msg->addr));

    *reply_msg = msg_init(NULL);
    (*reply_msg)->addr = msg->addr;
    strcpy((*reply_msg)->key, msg->key);

    int body_index = 0;
    for(int x = player->position.x - 4; x <= player->position.x + 4; x++)
    {
        for(int y = player->position.y - 4; y <= player->position.y + 4; y++)
        {
            ((int*)(*reply_msg)->body)[body_index] = (map + y * msize_x + x)->surface != empty
                                            ? (map + y * msize_x + x)->surface + GROUND_TYPE_CNT
                                            : (map + y * msize_x + x)->ground;
            body_index++;

            ((int*)(*reply_msg)->body)[body_index] = (map + y * msize_x + x)->child_object_type;
            body_index++;
        }
    }

    (*reply_msg)->len = sizeof(ground_type_t) * sizeof(object_type_t) * 81;
    (*reply_msg)->len += (size_t)sprintf((char*)( ((int*)(*reply_msg)->body) + body_index ), "[%i, %i]", player->position.x, player->position.y) + 1;

    (*reply_msg)->type = map_update;


}

static void gfunc_go(msg_t *msg, msg_t **reply_msg, char *args)
{
    character_t *player;

    player = character_get_by_addr(&(msg->addr));

    char *answ;


    char *direct = get_word(&args);
    if(direct == NULL)
    {
        *reply_msg = msg_init(NULL);
        (*reply_msg)->addr = msg->addr;
        strcpy((*reply_msg)->key, msg->key);
        (*reply_msg)->type = main_msg;

        (*reply_msg)->len = (size_t)sprintf((*reply_msg)->body, "You should specify the direction!%s", direct) + 1;
        return;
    }

    if( (strcmp(direct, "west") == 0) || (strcmp(direct, "right") == 0) )
    {
        player->next_step = west;
    }
    else if( (strcmp(direct, "east") == 0) || (strcmp(direct, "left") == 0) )
    {
        player->next_step = east;
    }
    else if( (strcmp(direct, "north") == 0) || (strcmp(direct, "up") == 0) )
    {
        player->next_step = south;
    }
    else if( (strcmp(direct, "south") == 0) || (strcmp(direct, "down") == 0) )
    {
        player->next_step = north;
    }
    else
    {
        *reply_msg = msg_init(NULL);
        (*reply_msg)->addr = msg->addr;
        strcpy((*reply_msg)->key, msg->key);
        (*reply_msg)->type = main_msg;

        answ = "You should specify the correct direction!";
        (*reply_msg)->len = strlen(answ);
        memcpy((*reply_msg)->body, answ, (*reply_msg)->len + 1);
        return;
    }

    if(character_move_to_target(player, game_get_msize_x(), game_get_msize_y(), game_get_map()) == nowhere)
    {
        *reply_msg = msg_init(NULL);
        (*reply_msg)->addr = msg->addr;
        strcpy((*reply_msg)->key, msg->key);
        (*reply_msg)->type = main_msg;

        (*reply_msg)->len = (size_t)sprintf((*reply_msg)->body, "You cannot go %s", direct) + 1;
        return;
    }

}

static void gfunc_hello(msg_t *msg, msg_t **reply_msg, char *args)
{
    *reply_msg = msg_init(NULL);
    (*reply_msg)->addr = msg->addr;
    strcpy((*reply_msg)->key, msg->key);

    (*reply_msg)->type = main_msg;
    (*reply_msg)->len = (size_t)sprintf((*reply_msg)->body, "Hello, %s!", (character_get_by_addr(&(msg->addr)))->name) + 1;
}

static void gfunc_dntknow(msg_t *msg, msg_t **reply_msg, char *args)
{
    char *answ = "I don't know...";

    *reply_msg = msg_init(NULL);
    (*reply_msg)->addr = msg->addr;
    strcpy((*reply_msg)->key, msg->key);

    (*reply_msg)->type = main_msg;
    (*reply_msg)->len = strlen(answ);
    memcpy((*reply_msg)->body, answ, (*reply_msg)->len + 1);
}

bool gfunc_init(char **err)
{
    srand(time(NULL));

    funcs_hashtable = str_hashtable_create(FUNCS_CNT);
    func_name_t funcs[FUNCS_CNT] =
            {
                    {gfunc_go, "go"},
                    {gfunc_say, "say"},
                    {gfunc_hello, "hello"},
                    {gfunc_hello, "hi"},
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

void (* gfunc_get(char *func_name) )(msg_t*, msg_t**, char*)
{
    void (*func)(msg_t*, msg_t**, char*);
    func = str_hashtable_get(funcs_hashtable, func_name);

    return (func != NULL) ? func : gfunc_dntknow;
}

void gfunc_destroy()
{
    str_hashtable_destroy(funcs_hashtable);
}