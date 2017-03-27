#include "character.h"
#include <limits.h>
#include <string.h>

extern double sqrt(double);
extern double pow(double, double);

character_t* character_new(int position_x, int position_y, char *name, character_race_t race, int lvl, clock_t step_time, bool is_player)
{
    character_t* new_char = malloc(sizeof(character_t));
    new_char->position.x = position_x;
    new_char->position.y = position_y;
    sprintf(new_char->name, "%s", name);
    new_char->aggression = false;
    new_char->step_time = step_time;
    new_char->target = NULL;
    new_char->next_step = nowhere;
    new_char->lvl = lvl;
    new_char->race = race;
    new_char->is_player = is_player;
    new_char->id = 0;
    new_char->step_start_time = 0;
    new_char->items.arr = NULL;
    new_char->items.count = 0;

    switch(race)
    {
        case human:
            new_char->characteristics.armor = 5;
            new_char->characteristics.evasion = 2;
            new_char->characteristics.power = 10;
            new_char->characteristics.hp = 100;
            break;

        case beast:
            new_char->characteristics.armor = 2;
            new_char->characteristics.evasion = 2;
            new_char->characteristics.power = 13;
            new_char->characteristics.hp = 100;
            break;
    }

    new_char->characteristics.armor *= lvl;
    new_char->characteristics.evasion *= lvl;
    new_char->characteristics.power *= lvl;
    new_char->characteristics.hp *= lvl;


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
    characters->arr = realloc(characters->arr, (characters->count + 1) * sizeof(void*));
    characters->arr[characters->count] = _character;
    _character->id = characters->count;
    characters->count++;

    (_map + _character->position.y * _msize_x + _character->position.x)->child_object_type = character;
}

void character_remove(characters_t *characters, character_t *character, size_t _msize_x, map_point_t* _map, int *index)
{
    (_map + character->position.y * _msize_x + character->position.x)->child_object_type = nothing;
    (_map + character->position.y * _msize_x + character->position.x)->child_object = NULL;

    if(characters->count - 1  !=  character->id)
    {
        if( (index != NULL) && ((*index) > character->id) )
            (*index)--;

        memmove(characters->arr + character->id, characters->arr + character->id + 1, (characters->count - character->id) * sizeof(void *));
    }

    characters->count--;
}

void character_find_target(character_t *character, characters_t *characters)
{
    double dist;
    double min_dist;

    if(character->is_player)
        return;

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

static int character_damage(character_t *character, character_t *target)
{
    int damage = character->characteristics.power - (rand() % target->characteristics.evasion) - target->characteristics.armor;
    if(damage < 0)
        damage = 0;

    target->characteristics.hp -= damage;
    if(target->characteristics.hp < 0)
        target->characteristics.hp = 0;

    return damage;
}

//TODO: add action and send to target
bool character_attack(character_t *character, characters_t *characters, size_t _msize_x, map_point_t* _map, int *index)
{
    double dist;

    if(character->target == NULL)
        return false;

    dist = sqrt( pow(character->target->position.x - character->position.x, 2) + pow(character->target->position.y - character->position.y, 2) );

    /*
     * DEBUG INFO

    printf("try attack %s(%i hp, %i lvl) to %s(%i hp, %i lvl), dist = %f\n",
           character->name, character->characteristics.hp, character->lvl, character->target->name, character->target->characteristics.hp, character->target->lvl, dist);
    */

    if(dist < 2)
    {

        character_damage(character, character->target);
        character->target->target = character;
        character->target->aggression = true;

        if(character->target->characteristics.hp <= 0)
        {
            character_remove(characters, character->target, _msize_x, _map, index);
            character->target = NULL;
        }

        if(character->is_player)
        {
            character->aggression = false;
            character->target = NULL;
        }

        return true;
    }

    return false;

}

void character_add_item(character_t *character, item_t *item)
{
    character->items.arr = realloc(character->items.arr, (character->items.count + 1)  * sizeof(void*) );
    character->items.arr[character->items.count] = item;
    character->items.count++;
}

direction_t character_move_to_target(character_t *character, size_t _msize_x, size_t _msize_y, map_point_t* _map)
{
    double dirs[4];
    direction_t direction;

    double dist;
    double min_dist;

    bool player_step_avaliable = true;

    if(character->is_player)
    {
        switch(character->next_step)
        {
            case west:
                player_step_avaliable = (character->position.x + 1 < _msize_x)
                                        &&  ((_map + character->position.y*_msize_x + character->position.x + 1)->surface != wall)
                                             && ((_map + character->position.y*_msize_x + character->position.x + 1)->child_object_type == nothing)
                                                && ((_map + character->position.y*_msize_x + character->position.x + 1)->ground < GROUND_TYPE_CNT);
                break;

            case east:
                player_step_avaliable = (character->position.x - 1 >= 0)
                                        &&  ((_map + character->position.y*_msize_x + character->position.x - 1)->surface != wall)
                                             && ((_map + character->position.y*_msize_x + character->position.x - 1)->child_object_type == nothing)
                                                && ((_map + character->position.y*_msize_x + character->position.x - 1)->ground < GROUND_TYPE_CNT);
                break;

            case north:
                player_step_avaliable = (character->position.y + 1 < _msize_y)
                                        &&  ((_map + (character->position.y + 1)*_msize_x + character->position.x)->surface != wall)
                                             && ((_map + (character->position.y + 1)*_msize_x + character->position.x)->child_object_type == nothing)
                                                && ((_map + (character->position.y + 1)*_msize_x + character->position.x)->ground < GROUND_TYPE_CNT);
                break;

            case south:
                player_step_avaliable = (character->position.y - 1 >= 0)
                                        &&  ((_map + (character->position.y - 1)*_msize_x + character->position.x)->surface != wall)
                                             && ((_map + (character->position.y - 1)*_msize_x + character->position.x)->child_object_type == nothing)
                                                && ((_map + (character->position.y - 1)*_msize_x + character->position.x)->ground < GROUND_TYPE_CNT);
                break;

            case nowhere:
                break;
        }

        return player_step_avaliable ? character->next_step : nowhere;
    }


    if(character->target == NULL)
        return nowhere;


    min_dist = INT_MAX;
    for(int x = character->position.x + 1; (x < _msize_x) && (x <= character->position.x + visibility_range) ; x++)
    {
        if( ((_map + character->position.y*_msize_x + x)->surface == wall) || ((_map + character->position.y*_msize_x + x)->child_object_type != nothing)
                || ((_map + character->position.y*_msize_x + x)->ground >= GROUND_TYPE_CNT))
            break;

        dist = sqrt( pow(character->target->position.x - x, 2) + pow(character->target->position.y - character->position.y, 2) );
        if(dist < min_dist)
            min_dist = dist;
    }
    dirs[west] = min_dist;

    min_dist = INT_MAX;
    for(int x = character->position.x - 1; (x >= 0) && (x >= character->position.x - visibility_range) ; x--)
    {
        if( ((_map + character->position.y*_msize_x + x)->surface == wall) || ((_map + character->position.y*_msize_x + x)->child_object_type != nothing)
                || ((_map + character->position.y*_msize_x + x)->ground >= GROUND_TYPE_CNT))
            break;

        dist = sqrt( pow(character->target->position.x - x, 2) + pow(character->target->position.y - character->position.y, 2) );
        if(dist < min_dist)
            min_dist = dist;
    }
    dirs[east] = min_dist;

    min_dist = INT_MAX;
    for(int y = character->position.y + 1; (y < _msize_y) && (y <= character->position.y + visibility_range) ; y++)
    {
        if( ((_map + y*_msize_x + character->position.x)->surface == wall) || ((_map + y*_msize_x + character->position.x)->child_object_type != nothing)
                || ((_map + y*_msize_x + character->position.x)->ground >= GROUND_TYPE_CNT))
            break;

        dist = sqrt( pow(character->target->position.y - y, 2) + pow(character->target->position.x - character->position.x, 2) );
        if(dist < min_dist)
            min_dist = dist;
    }
    dirs[north] = min_dist;

    min_dist = INT_MAX;
    for(int y = character->position.y - 1; (y >= 0) && (y >= character->position.y - visibility_range) ; y--)
    {
        if( ((_map + y*_msize_x + character->position.x)->surface == wall) || ((_map + y*_msize_x + character->position.x)->child_object_type != nothing)
                || ((_map + y*_msize_x + character->position.x)->ground >= GROUND_TYPE_CNT))
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

