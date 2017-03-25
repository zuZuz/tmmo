#include "game_main.h"
#include <unistd.h>

union_buildings_t barracks = {NULL, 0};
union_buildings_t smithies = {NULL, 0};
union_buildings_t markets = {NULL, 0};
union_buildings_t taverns = {NULL, 0};
union_buildings_t academies = {NULL, 0};

characters_t characters;

map_point_t* _map;
size_t _msize_x;
size_t _msize_y;

static void game_tick();

void game_init(map_point_t *map, size_t msize_x, size_t msize_y, building_t *buildings, size_t buildings_cnt)
{
    _map = map;
    _msize_x = msize_x;
    _msize_y = msize_y;

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


    while(1)
    {
        game_tick();

        usleep(tick_delay);
    }
}


static void game_tick()
{
    for(int i = 0; i < characters.count; i++) {

        if ( (characters.arr[i]->next_step == nowhere)
             || ( (clock() - characters.arr[i]->step_start_time) <= characters.arr[i]->step_time) )
            continue;

        characters.arr[i]->next_step = character_move_to_target(characters.arr[i], _msize_x, _msize_y, _map);
        characters.arr[i]->step_start_time = clock();

        switch (characters.arr[i]->next_step) {
            case west:
                characters.arr[i]->position.x++;
                break;

            case east:
                characters.arr[i]->position.x--;
                break;

            case north:
                characters.arr[i]->position.y++;
                break;

            case south:
                characters.arr[i]->position.y--;
                break;
        }

        (_map + characters.arr[i]->position.y * _msize_x + characters.arr[i]->position.x)->child_object = character;


        /*
         * DEBUG INFO
         *
        printf("character \"%s\" pos: %i, %i, target: %p, child_object: %i, next_step: %i\n", characters.arr[i]->name, characters.arr[i]->position.x, characters.arr[i]->position.y,
               characters.arr[i]->target,
               (_map + characters.arr[i]->position.y * _msize_x + characters.arr[i]->position.x)->child_object, characters.arr[i]->next_step );

        */
    }
}


