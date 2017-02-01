/*
 * Initialization of game functions and return a function by its name.
 */

#include "../server/network.h"
#include <stdbool.h>

#ifndef GAME_FUNCTIONS_H
#define GAME_FUNCTIONS_H

/*
 * returns false if it was not possible to initialize,
 * err contains incorrect function name
 */
bool gfunc_init(char **err);

void (* gfunc_get(char *func_name) )(msg_t*, char*);

#endif //GAME_FUNCTIONS_H
