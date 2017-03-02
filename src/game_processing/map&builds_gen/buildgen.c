#include "../map.h"
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "buildgen.h"


void add_on_map(FILE *fmap, FILE *fbuild)
{
    printf("add_on_map\n");

    srand(time(NULL));

    map_point_t *build, *map;
    building_t new_building;
    building_t *old_buildings = NULL;
    size_t bsize_x, bsize_y;
    size_t msize_x, msize_y;
    size_t buildings_cnt;
    size_t x, y;
    bool have_place;
    size_t region;

    fread(&bsize_x, sizeof(size_t), 1, fbuild);
    fread(&bsize_y, sizeof(size_t), 1, fbuild);
    build = malloc( bsize_y * bsize_x * sizeof(map_point_t) );
    fread(build, bsize_y * bsize_x * sizeof(map_point_t), 1, fbuild);

    fread(&buildings_cnt, sizeof(size_t), 1, fmap);
    if(buildings_cnt != 0)
    {
        old_buildings = malloc( buildings_cnt * sizeof(building_t) );
        fread(old_buildings, buildings_cnt * sizeof(building_t), 1, fmap);
    }

    fread(&msize_x, sizeof(size_t), 1, fmap);
    msize_y = msize_x;
    map = malloc( msize_y * msize_x * sizeof(map_point_t) );
    fread(map, msize_x * msize_y * sizeof(map_point_t), 1, fmap);

    printf("region: ");
    while (scanf("%zu", &region) == 0);


    do
    {
        have_place = true;

        x =  msize_x * (double)rand() / RAND_MAX;
        y =  msize_y * (double)rand() / RAND_MAX;

        for (size_t yy = y; yy < y + bsize_y; yy++)
        {
            for (size_t xx = x; xx < x + bsize_x; xx++)
            {
                if((map + yy*msize_x + xx)->ground != build->ground  ||
                        (map + yy*msize_x + xx)->surface != empty ||
                        (map + yy*msize_x + xx)->region != region )
                {
                    have_place = false;
                    break;
                }
            }

            if(!have_place)
                break;
        }

    } while(!have_place);


    int i = 0;
    for (size_t yy = y; yy < y + bsize_y; yy++)
    {
        for (size_t xx = x; xx < x + bsize_x; xx++)
        {
            (map + yy*msize_x + xx)->surface = (build + i)->surface;
	    (map + yy*msize_x + xx)->name_id = (build + i)->name_id;
            i++;
        }
    }

    new_building.point = *(map + y*msize_x + x);
    new_building.size_x = bsize_x;
    new_building.size_y = bsize_y;

    printf("x: %zu, y: %zu\n", x, y);
    fseek(fmap, 0, SEEK_SET);

    buildings_cnt++;

    fwrite(&buildings_cnt, sizeof(size_t), 1, fmap);
    if(old_buildings != NULL)
        fwrite(old_buildings, (buildings_cnt-1) * sizeof(building_t), 1, fmap);
    fwrite(&new_building, sizeof(building_t), 1, fmap);

    fwrite(&msize_x, sizeof(size_t), 1, fmap);
    fwrite(map, msize_y * msize_x * sizeof(map_point_t), 1, fmap);
    fflush(fmap);
}

void buildgen()
{
    int nameid;
    char ch;
    char buf[255];
    size_t size_x, size_y;
    FILE *f;

    printf("nameid: ");
    while (scanf("%i", &nameid) == 0);

    printf("size_x: ");
    while (scanf("%zu", &size_x) == 0);

    printf("size_y: ");
    while (scanf("%zu", &size_y) == 0);

    map_point_t *build = malloc( size_y * size_x * sizeof(map_point_t) );

    printf("w - wall, f - floor (coord x&y mod 10)\n");

    printf(" ");
    for(int i = 0; i < size_x; i++)
    {
        printf(" %i", i % 10);
    }
    printf("\n");

    for (int y = 0; y < size_y; y++)
    {
        printf("%i ", y % 10);

        for (int x = 0; x < size_x; x++)
        {
            while ((scanf("%c", &ch) == 0) ||
                    (ch != 'w' && ch != 'f') );

            switch(ch)
            {
                case 'w': (build + y*size_x + x)->surface = wall;
                    break;

                case 'f': (build + y*size_x + x)->surface = floor;
                    break;
            }

            (build + y*size_x + x)->ground = soil;
            (build + y*size_x + x)->name_id = nameid;
            (build + y*size_x + x)->child_object = nothing;
            (build + y*size_x + x)->x = x;
            (build + y*size_x + x)->y = y;
        }
    }


    printf("Build: \n");
    for (size_t y = 0; y < size_y; y++)
    {
        for (size_t x = 0; x < size_x; x++)
        {
            switch((build + y*size_x + x)->surface)
            {
                case wall: printf("w ");
                    break;

                case floor: printf("f ");
                    break;

                default: printf("X ");
            }
        }

        printf("\n");
    }

    sprintf(buf, "%i_%zu.build", nameid, size_x*size_y);

    f = fopen(buf, "wb");

    fwrite(&size_x, sizeof(size_t), 1, f);
    fflush(f);
    fwrite(&size_y, sizeof(size_t), 1, f);
    fflush(f);

    fwrite(build, size_y * size_x * sizeof(map_point_t), 1, f);
    fflush(f);

}
