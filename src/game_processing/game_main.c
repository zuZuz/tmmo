#include "game_main.h"
#include "item.h"
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

    character_t *player1 = character_new(180, 180, "player1", human, 1, 5000, false);
    character_t *player2 = character_new(190, 190, "player2", beast, 1, 5000, false);

    characteristics_t characs;
    characs.armor = 10;
    characs.evasion = 10;
    characs.hp = 10;
    characs.power = 10;

    character_add_item(player1, item_new(onehanded_sword, "одноручный меч дракона", characs, 100));
    character_add_item(player1, item_new(twohanded_sword, "двуручный меч дракона", characs, 100));
    character_add_item(player2, item_new(shield, "щит братьев гор", characs, 100));



    player1->aggression = true;
    player2->aggression = true;

    character_add(&characters, player1, msize_x, msize_y, map);
    character_add(&characters, player2, msize_x, msize_y, map);


    while(1)
    {
        game_tick();

        usleep(tick_delay);
    }
}


static void game_tick()
{
    for(int i = 0; i < characters.count; i++) {

        if ( (characters.arr[i]->next_step == nowhere && !characters.arr[i]->aggression)
             || ( (clock() - characters.arr[i]->step_start_time) <= characters.arr[i]->step_time) )
            continue;

        (_map + characters.arr[i]->position.y * _msize_x + characters.arr[i]->position.x)->child_object_type = nothing;
        (_map + characters.arr[i]->position.y * _msize_x + characters.arr[i]->position.x)->child_object = NULL;

        character_find_target(characters.arr[i], &characters);
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

            case nowhere:
                character_attack(characters.arr[i], &characters);
                break;
        }

        (_map + characters.arr[i]->position.y * _msize_x + characters.arr[i]->position.x)->child_object_type = character;
        (_map + characters.arr[i]->position.y * _msize_x + characters.arr[i]->position.x)->child_object = characters.arr[i];


        /*
         * DEBUG INFO
         **/
        printf("after character moved \"%s\" pos: %i, %i, target: %p, child_object: %i, next_step: %i\n", characters.arr[i]->name, characters.arr[i]->position.x, characters.arr[i]->position.y,
               characters.arr[i]->target,
               (_map + characters.arr[i]->position.y * _msize_x + characters.arr[i]->position.x)->child_object_type, characters.arr[i]->next_step );

        for(int j = 0; j < characters.arr[i]->items.count; j++)
        {
            printf("%i:%i) %s (%i)\n", j + 1, characters.arr[i]->items.count, characters.arr[i]->items.arr[j]->name, characters.arr[i]->items.arr[j]->type);
        }


    }
}


