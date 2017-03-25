#include <stdio.h>
#include <stdlib.h>
#include "../map&builds_gen/buildgen.h"
#include "game_main.h"


int main(int argc, char **argv)
{
    FILE *fmap;

    size_t msize_x, msize_y;
    size_t buildings_cnt;
    building_t *buildings;

    if(argc < 2)
    {
        printf("mapfile should be specified\n");
        return 1;
    }

    fmap = fopen(argv[1], "r");
    if (fmap == NULL)
    {
        printf("can't open mapfile: %s\n", argv[1]);
        return 1;
    }

    fread(&buildings_cnt, sizeof(size_t), 1, fmap);
    if(buildings_cnt != 0)
    {
        buildings = malloc( buildings_cnt * sizeof(building_t) );
        fread(buildings, buildings_cnt * sizeof(building_t), 1, fmap);
    }

    fread(&msize_x, sizeof(size_t), 1, fmap);
    msize_y = msize_x;

    map_point_t *map = malloc( msize_y * msize_x * sizeof(map_point_t) );

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

    game_init(map, msize_x, msize_y, buildings, buildings_cnt);
}
