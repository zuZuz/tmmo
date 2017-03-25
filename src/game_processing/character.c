#include "character.h"
#include <limits.h>
#include <string.h>

extern double sqrt(double);
extern double pow(double, double);

character_t* character_new(int position_x, int position_y, char *name, clock_t step_time)
{
    character_t* new_char = malloc(sizeof(character_t));
    new_char->position.x = position_x;
    new_char->position.y = position_y;
    new_char->name = name;
    new_char->step_time = step_time;
}

void character_set_target(character_t *character, map_point_t *target)
{
    character->target = target;
    character->next_step = 0;
    character->step_start_time = 0;
}

void character_add(characters_t *characters, character_t *character)
{
    character->id = characters->count;
    characters->arr = realloc(characters->arr, characters->count + 1);
    characters->arr[characters->count] = character;
    characters->count++;
}

void character_remove(characters_t *characters, character_t *character)
{
    memmove(characters->arr + character->id, characters->arr + character->id + 1, (characters->count - character->id) * sizeof(void *));
    characters->count--;
}

direction_t character_move_to_target(character_t *character, size_t _msize_x, size_t _msize_y, map_point_t* _map)
{
    double dirs[4];
    direction_t direction;

    double dist;
    double min_dist;

    min_dist = INT_MAX;
    for(int x = character->position.x + 1; (x < _msize_x) && (x <= character->position.x + visibility_range) ; x++)
    {
        if( (_map + character->position.y*_msize_x + x)->surface != empty )
            continue;

        dist = sqrt( pow(character->target->x - x, 2) + pow(character->target->y - character->position.y, 2) );
        if(dist < min_dist)
            min_dist = dist;
    }
    dirs[west] = min_dist;

    min_dist = INT_MAX;
    for(int x = character->position.x - 1; (x >= 0) && (x >= character->position.x - visibility_range) ; x--)
    {
        if( (_map + character->position.y*_msize_x + x)->surface != empty )
            continue;

        dist = sqrt( pow(character->target->x - x, 2) + pow(character->target->y - character->position.y, 2) );
        if(dist < min_dist)
            min_dist = dist;
    }
    dirs[east] = min_dist;

    min_dist = INT_MAX;
    for(int y = character->position.y + 1; (y < _msize_y) && (y <= character->position.y + visibility_range) ; y++)
    {
        if( (_map + y*_msize_x + character->position.x)->surface != empty )
            continue;

        dist = sqrt( pow(character->target->y - y, 2) + pow(character->target->x - character->position.x, 2) );
        if(dist < min_dist)
            min_dist = dist;
    }
    dirs[north] = min_dist;

    min_dist = INT_MAX;
    for(int y = character->position.y - 1; (y >= 0) && (y >= character->position.y - visibility_range) ; y--)
    {
        if( (_map + y*_msize_x + character->position.x)->surface != empty )
            continue;

        dist = sqrt( pow(character->target->y - y, 2) + pow(character->target->x - character->position.x, 2) );
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
    dist = sqrt( pow(character->target->y - character->position.y, 2) + pow(character->target->x - character->position.x, 2) );
    if(dist < min_dist || dist <= 1)
        direction = nowhere;

    return direction;
}

