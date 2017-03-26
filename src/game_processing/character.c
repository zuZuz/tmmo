#include "character.h"
#include <limits.h>
#include <string.h>

extern double sqrt(double);
extern double pow(double, double);

character_t* character_new(int position_x, int position_y, char *name, character_race_t race, int hp, int lvl, clock_t step_time)
{
    character_t* new_char = malloc(sizeof(character_t));
    new_char->position.x = position_x;
    new_char->position.y = position_y;
    new_char->name = name;
    new_char->aggression = false;
    new_char->step_time = step_time;
    new_char->target = NULL;
    new_char->next_step = nowhere;
    new_char->hp = hp;
    new_char->lvl = lvl;
    new_char->race = race;

    return new_char;
}

void character_set_target(character_t *character, character_t *target)
{
    character->target = target;
    character->next_step = 0;
    character->step_start_time = 0;
}

void character_add(characters_t *characters, character_t *_character, size_t _msize_x, size_t _msize_y, map_point_t* _map)
{
    _character->id = characters->count;
    characters->arr = realloc(characters->arr, characters->count + 1);
    characters->arr[characters->count] = _character;
    characters->count++;

    (_map + _character->position.y * _msize_x + _character->position.x)->child_object_type = character;
}

void character_remove(characters_t *characters, character_t *character)
{
    memmove(characters->arr + character->id, characters->arr + character->id + 1, (characters->count - character->id) * sizeof(void *));
    characters->count--;
}

void character_find_target(character_t *character, characters_t *characters)
{
    double dist;
    double min_dist;

    min_dist = INT_MAX;
    for(int i = 0; i < characters->count; i++)
    {
        if( (characters->arr[i] == character) || (characters->arr[i]->race == character->race) )
            continue;

        dist = sqrt( pow(characters->arr[i]->position.x - character->position.x, 2) + pow(characters->arr[i]->position.y - character->position.y, 2) );
        if(dist < min_dist)
        {
            min_dist = dist;
            character_set_target(character, characters->arr[i]);
        }

    }
}

//TODO: add action and send to target
void character_attack(character_t *character, characters_t *characters)
{
    double dist;

    if(character->target == NULL)
        return;

    dist = sqrt( pow(character->target->position.x - character->position.x, 2) + pow(character->target->position.y - character->position.y, 2) );

    /*
     * DEBUG INFO
     */
    printf("try attack %s(%i hp, %i lvl) to %s(%i hp, %i lvl), dist = %f\n",
           character->name, character->hp, character->lvl, character->target->name, character->target->hp, character->target->lvl, dist);


    if(dist < 2)
    {
        character->target->hp -= rand() % character->lvl;
        if(character->target->hp <= 0)
        {
            character_remove(characters, character->target);
            character->target = NULL;
        }
    }

}

direction_t character_move_to_target(character_t *character, size_t _msize_x, size_t _msize_y, map_point_t* _map)
{
    double dirs[4];
    direction_t direction;

    double dist;
    double min_dist;

    if(character->target == NULL)
        return nowhere;

    min_dist = INT_MAX;
    for(int x = character->position.x + 1; (x < _msize_x) && (x <= character->position.x + visibility_range) ; x++)
    {
        if( ((_map + character->position.y*_msize_x + x)->surface != empty) || ((_map + character->position.y*_msize_x + x)->child_object_type != nothing) )
            break;

        dist = sqrt( pow(character->target->position.x - x, 2) + pow(character->target->position.y - character->position.y, 2) );
        if(dist < min_dist)
            min_dist = dist;
    }
    dirs[west] = min_dist;

    min_dist = INT_MAX;
    for(int x = character->position.x - 1; (x >= 0) && (x >= character->position.x - visibility_range) ; x--)
    {
        if( ((_map + character->position.y*_msize_x + x)->surface != empty) || ((_map + character->position.y*_msize_x + x)->child_object_type != nothing) )
            break;

        dist = sqrt( pow(character->target->position.x - x, 2) + pow(character->target->position.y - character->position.y, 2) );
        if(dist < min_dist)
            min_dist = dist;
    }
    dirs[east] = min_dist;

    min_dist = INT_MAX;
    for(int y = character->position.y + 1; (y < _msize_y) && (y <= character->position.y + visibility_range) ; y++)
    {
        if( ((_map + y*_msize_x + character->position.x)->surface != empty) || ((_map + y*_msize_x + character->position.x)->child_object_type != nothing) )
            break;

        dist = sqrt( pow(character->target->position.y - y, 2) + pow(character->target->position.x - character->position.x, 2) );
        if(dist < min_dist)
            min_dist = dist;
    }
    dirs[north] = min_dist;

    min_dist = INT_MAX;
    for(int y = character->position.y - 1; (y >= 0) && (y >= character->position.y - visibility_range) ; y--)
    {
        if( ((_map + y*_msize_x + character->position.x)->surface != empty) || ((_map + y*_msize_x + character->position.x)->child_object_type != nothing) )
            break;

        dist = sqrt( pow(character->target->position.y - y, 2) + pow(character->target->position.x - character->position.x, 2) );
        if(dist < min_dist)
            min_dist = dist;
    }
    dirs[south] = min_dist;


    min_dist = INT_MAX;
    for(int i = 0; i < 4; i++)
    {
        if(dirs[i] < min_dist)
        {
            min_dist = dirs[i];
            direction = i;
        }
    }
    dist = sqrt( pow(character->target->position.y - character->position.y, 2) + pow(character->target->position.x - character->position.x, 2) );
    if(dist < min_dist || dist <= 1)
        direction = nowhere;

    return direction;
}

