/*
 * Initialization of game functions and return a function by its name.
 */

#include "../server/network.h"

#ifndef GAME_FUNCTIONS_H
#define GAME_FUNCTIONS_H

void gfunc_init();

void (* gfunc_get(char *func_name) )(msg_t*, char*);

#endif //GAME_FUNCTIONS_H
