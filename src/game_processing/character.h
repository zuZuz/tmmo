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

typedef struct characteristics
{
    int power;
    int armor;
    int evasion;
    int hp;
} characteristics_t;


typedef enum item_type
{
    onehanded_sword,
    twohanded_sword,
    shield,
    full_armor
} item_type_t;

typedef struct item
{
    item_type_t type;
    char *name;
    characteristics_t characteristics;
    int strength;
} item_t;

typedef struct items
{
    item_t **arr;
    int count;

} items_t;


typedef struct character
{
    char *name;
    character_race_t race;
    int id;
    int lvl;
    characteristics_t characteristics;
    map_point_t position;
    bool is_player;
    items_t items;

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


character_t* character_new(int position_x, int position_y, char *name, character_race_t race, int lvl, clock_t step_time, bool is_player);
void character_set_target(character_t *character, character_t *target);
direction_t character_move_to_target(character_t *character, size_t _msize_x, size_t _msize_y, map_point_t* _map);
void character_add(characters_t *characters, character_t *_character, size_t _msize_x, size_t _msize_y, map_point_t* _map);
void character_remove(characters_t *characters, character_t *character, size_t _msize_x, map_point_t* _map, int *index);
bool character_attack(character_t *character, characters_t *characters, size_t _msize_x, map_point_t* _map, int *index);
void character_find_target(character_t *character, characters_t *characters);
void character_add_item(character_t *character, item_t *item);


#endif //CHARACTER_H
