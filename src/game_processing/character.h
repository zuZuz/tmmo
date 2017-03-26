#include <time.h>
#include <stdbool.h>
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


typedef enum character_race
{
    human = 0,
    beast

} character_race_t;

typedef struct character
{
    char *name;
    character_race_t race;
    int id;
    int hp;
    int lvl;
    map_point_t position;
    bool is_player;

    clock_t step_time;

    struct character *target;
    bool aggression;
    direction_t next_step;
    clock_t step_start_time;

} character_t;

typedef struct characters
{
    character_t **arr;
    int count;

} characters_t;


character_t* character_new(int position_x, int position_y, char *name, character_race_t race, int hp, int lvl, clock_t step_time, bool is_player);
void character_set_target(character_t *character, character_t *target);
direction_t character_move_to_target(character_t *character, size_t _msize_x, size_t _msize_y, map_point_t* _map);
void character_add(characters_t *characters, character_t *_character, size_t _msize_x, size_t _msize_y, map_point_t* _map);
void character_remove(characters_t *characters, character_t *character);
bool character_attack(character_t *character, characters_t *characters);
void character_find_target(character_t *character, characters_t *characters);


#endif //CHARACTER_H
