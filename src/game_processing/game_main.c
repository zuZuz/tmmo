#include "game_main.h"
#include "item.h"
#include "query_processing.h"
#include <stdbool.h>
#include <unistd.h>
#include <memory.h>

union_buildings_t barracks = {NULL, 0};
union_buildings_t smithies = {NULL, 0};
union_buildings_t markets = {NULL, 0};
union_buildings_t taverns = {NULL, 0};
union_buildings_t academies = {NULL, 0};

characters_t characters;

map_point_t* map;
size_t msize_x, msize_y;
size_t buildings_cnt;
building_t *buildings;
int character_index_tick;

static void game_tick();


bool game_init(char *map_name)
{
    FILE *fmap;

    if(map_name == NULL)
    {
        printf("mapfile should be specified\n");
        return false;
    }

    fmap = fopen(map_name, "r");
    if (fmap == NULL)
    {
        return false;
    }

    fread(&buildings_cnt, sizeof(size_t), 1, fmap);
    if(buildings_cnt != 0)
    {
        buildings = malloc( buildings_cnt * sizeof(building_t) );
        fread(buildings, buildings_cnt * sizeof(building_t), 1, fmap);
    }

    fread(&msize_x, sizeof(size_t), 1, fmap);
    msize_y = msize_x;

    map = malloc( msize_y * msize_x * sizeof(map_point_t) );

    fread(map, msize_x * msize_y * sizeof(map_point_t), 1, fmap);

    /*
     * DEBUG INFO
     */
    printf("mapsize x: %zu, y: %zu\n", msize_x, msize_y);
    for(int i = 0; i < buildings_cnt; i++)
    {
        printf("buildname_id: %i, coord: (%i, %i), size_x: %zu, size_y: %zu\n",
               buildings[i].point.name_id, buildings[i].point.x, buildings[i].point.y, buildings[i].size_x, buildings->size_y);
    }

    return true;
}


void game_start(bool *is_stopped)
{

    for(int i = 0; i < buildings_cnt; i++)
    {
        switch(buildings[i].point.name_id / BUILDING_TYPE_OFFSET)
        {
            case 0: barracks.buildings = realloc(barracks.buildings, ++barracks.buildings_cnt * sizeof(building_t));
                *(barracks.buildings + barracks.buildings_cnt - 1) = buildings[i];
                break;

            case 1: smithies.buildings = realloc(smithies.buildings, ++smithies.buildings_cnt * sizeof(building_t));
                *(smithies.buildings + smithies.buildings_cnt - 1) = buildings[i];
                break;

            case 2: markets.buildings = realloc(markets.buildings, ++markets.buildings_cnt * sizeof(building_t));
                *(markets.buildings + markets.buildings_cnt - 1) = buildings[i];
                break;

            case 3: taverns.buildings = realloc(taverns.buildings, ++taverns.buildings_cnt * sizeof(building_t));
                *(taverns.buildings + taverns.buildings_cnt - 1) = buildings[i];
                break;

            case 4: academies.buildings = realloc(academies.buildings, ++academies.buildings_cnt * sizeof(building_t));
                *(academies.buildings + academies.buildings_cnt - 1) = buildings[i];
                break;

        }
    }

    characters.count = 0;

    /*
     * DEBUG INFO
     */
    printf("barracks_cnt: %zu\n", barracks.buildings_cnt);
    printf("smithies_cnt: %zu\n", smithies.buildings_cnt);
    printf("markets: %zu\n", markets.buildings_cnt);
    printf("taverns: %zu\n", taverns.buildings_cnt);
    printf("academies: %zu\n", academies.buildings_cnt);


    while( !(*is_stopped) )
    {
        game_tick();

        usleep(tick_delay);
    }
}


static void game_tick()
{
    for(character_index_tick = 0; character_index_tick < characters.count; character_index_tick++)
    {

        if ( (characters.arr[character_index_tick]->next_step == nowhere && !characters.arr[character_index_tick]->aggression)
             || ( (clock() - characters.arr[character_index_tick]->step_start_time) <= characters.arr[character_index_tick]->step_time) )
            continue;

        (map + characters.arr[character_index_tick]->position.y * msize_x + characters.arr[character_index_tick]->position.x)->child_object_type = nothing;
        (map + characters.arr[character_index_tick]->position.y * msize_x + characters.arr[character_index_tick]->position.x)->child_object = NULL;

        character_find_target(characters.arr[character_index_tick], &characters);
        characters.arr[character_index_tick]->next_step = character_move_to_target(characters.arr[character_index_tick], msize_x, msize_y, map);
        characters.arr[character_index_tick]->step_start_time = clock();

        switch (characters.arr[character_index_tick]->next_step)
        {
            case west:
                characters.arr[character_index_tick]->position.x++;
                break;

            case east:
                characters.arr[character_index_tick]->position.x--;
                break;

            case north:
                characters.arr[character_index_tick]->position.y++;
                break;

            case south:
                characters.arr[character_index_tick]->position.y--;
                break;

            case nowhere:
                character_attack(characters.arr[character_index_tick], &characters, msize_x, map);
                break;
        }

        (map + characters.arr[character_index_tick]->position.y * msize_x + characters.arr[character_index_tick]->position.x)->child_object_type = characters.arr[character_index_tick]->is_player ? player : enemy;
        (map + characters.arr[character_index_tick]->position.y * msize_x + characters.arr[character_index_tick]->position.x)->child_object = characters.arr[character_index_tick];

        if(characters.arr[character_index_tick]->is_player)
        {
            characters.arr[character_index_tick]->next_step = nowhere;
            characters.arr[character_index_tick]->aggression = false;
        }


        for(int x = characters.arr[character_index_tick]->position.x - 4; x <= characters.arr[character_index_tick]->position.x + 4; x++)
        {
            for(int y = characters.arr[character_index_tick]->position.y - 4; y <= characters.arr[character_index_tick]->position.y + 4; y++)
            {
                if( (map + y * msize_x + x)->child_object_type != nothing)
                {
                    if( ((map + y * msize_x + x)->child_object != NULL) && ( ((character_t*)((map + y * msize_x + x)->child_object))->is_player ) )
                    {
                        character_t *charac = (character_t*)((map + y * msize_x + x)->child_object);

                        msg_t *message;

                        message = msg_init(NULL);
                        message->addr = *(charac->addr);
                        message->type = map_update;

                        query_processing_new(message);

                        message = msg_init(NULL);
                        message->addr = *(charac->addr);
                        message->type = char_info;

                        query_processing_new(message);

                        message = msg_init(NULL);
                        message->addr = *(charac->addr);
                        message->type = online_list;
                        message->body[0] = 0;

                        for(int j = 0; j < characters.count; j++)
                        {
                            if ( !(characters.arr[j]->is_player) || (strlen(characters.arr[j]->name) == 0) )
                                continue;

                            strcat(message->body, characters.arr[j]->name);
                            strcat(message->body, "\n");
                        }

                        message->len = strlen(message->body) + 1;

                        if(message->len > 1)
                            query_processing_new(message);
                    }
                }
            }
        }


        /*
         * DEBUG INFO
         *
        printf("after character moved \"%s\" pos: %i, %i, target: %p, child_object: %i, next_step: %i\n", characters.arr[character_index_tick]->name, characters.arr[character_index_tick]->position.x, characters.arr[character_index_tick]->position.y,
               characters.arr[character_index_tick]->target,
               (map + characters.arr[character_index_tick]->position.y * msize_x + characters.arr[character_index_tick]->position.x)->child_object_type, characters.arr[character_index_tick]->next_step );

        for(int j = 0; j < characters.arr[character_index_tick]->items.count; j++)
        {
            printf("%i:%i) %s (%i)\n", j + 1, characters.arr[character_index_tick]->items.count, characters.arr[character_index_tick]->items.arr[j]->name, characters.arr[character_index_tick]->items.arr[j]->type);
        }
            */



    }
}

characters_t* game_get_characters()
{
    return &characters;
}

int* game_get_character_index_tick()
{
    return &character_index_tick;
};

size_t game_get_msize_x()
{
    return msize_x;
}

size_t game_get_msize_y()
{
    return msize_y;
}

map_point_t* game_get_map()
{
    return map;
}


