/*
 * Initialization of game functions and return a function by its name.
 */

#include "../server/network.h"
#include <stdbool.h>

#ifndef GAME_FUNCTIONS_H
#define GAME_FUNCTIONS_H


typedef struct char_info {
    char    nick[16],
            lvl[16],
            hp[16],
            atp[16],
            arm[16],
            evn[16];
}char_info_t;

/*
 * returns false if it was not possible to initialize,
 * err contains incorrect function name
 */
bool gfunc_init(char **err);

void (* gfunc_get(char *func_name) )(msg_t*, msg_t**, char*);

void gfunc_destroy();

#endif //GAME_FUNCTIONS_H
