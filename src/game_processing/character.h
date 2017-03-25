#include <time.h>
#include "map.h"

#ifndef CHARACTER_H
#define CHARACTER_H

#define visibility_range 3

typedef enum direction
{
    west = 0,
    east,
    north,
    south,
    nowhere

} direction_t;

typedef struct character
{
    char *name;
    int id;
    u_int16_t hp;
    map_point_t position;

    clock_t step_time;

    map_point_t *target;
    direction_t next_step;
    clock_t step_start_time;

} character_t;

typedef struct characters
{
    character_t **arr;
    int count;

} characters_t;


character_t* character_new(int position_x, int position_y, char *name, clock_t step_time);
void character_set_target(character_t *character, map_point_t *target);
direction_t character_move_to_target(character_t *character, size_t _msize_x, size_t _msize_y, map_point_t* _map);
void character_add(characters_t *characters, character_t *character);
void character_remove(characters_t *characters, character_t *character);


#endif //CHARACTER_H
