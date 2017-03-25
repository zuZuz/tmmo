#include "game_main.h"

typedef struct union_buildings
{
    building_t *buildings;
    size_t buildings_cnt;
} union_buildings_t;



typedef struct character
{
    char name[255];
    u_int32_t id;
    map_point_t position;
    direction_t next_step;
    u_int16_t next_step_progress;
    u_int16_t hp;
    u_int16_t speed;
} character_t;


union_buildings_t barracks = {NULL, 0};
union_buildings_t smithies = {NULL, 0};
union_buildings_t markets = {NULL, 0};
union_buildings_t taverns = {NULL, 0};
union_buildings_t academies = {NULL, 0};

void game_init(map_point_t *map, size_t msize_x, size_t msize_y, building_t *buildings, size_t buildings_cnt)
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

    free(buildings);
}


