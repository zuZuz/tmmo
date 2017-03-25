#include "map.h"
#include "map&builds_gen/buildgen.h"

#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#define BUILDING_TYPE_OFFSET 10
typedef enum building_type
{
    barrack = 0,
    smithy,
    market,
    tavern,
    academy

} building_type_t;

typedef enum direction
{
    nowhere,
    right,
    left,
    up,
    down

} direction_t;

void game_init(map_point_t *map, size_t msize_x, size_t msize_y, building_t *buildings, size_t buildings_cnt);

#endif //GAME_MAIN_H
