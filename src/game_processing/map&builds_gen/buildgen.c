#include "../map.h"
#include <string.h>
#include <stdbool.h>
#include <time.h>

void add_on_map(FILE *fmap, FILE *fbuild)
{
    printf("add_on_map\n");

    srand(time(NULL));

    map_point_t *build, *map;
    size_t bsize_x, bsize_y;
    size_t msize_x, msize_y;
    size_t x, y;
    bool have_place;

    fread(&bsize_x, sizeof(size_t), 1, fbuild);
    fread(&bsize_y, sizeof(size_t), 1, fbuild);
    build = malloc( bsize_y * bsize_x * sizeof(map_point_t) );
    fread(build, bsize_y * bsize_x * sizeof(map_point_t), 1, fbuild);

    fread(&msize_x, sizeof(size_t), 1, fmap);
    msize_y = msize_x;
    map = malloc( msize_y * msize_x * sizeof(map_point_t) );
    fread(map, msize_x * msize_y * sizeof(map_point_t), 1, fmap);

    do
    {
        have_place = true;

        x =  msize_x * (double)rand() / RAND_MAX;
        y =  msize_y * (double)rand() / RAND_MAX;

        for (size_t yy = y; yy < y + bsize_y; yy++)
        {
            for (size_t xx = x; xx < x + bsize_x; xx++)
            {
                if((map + yy*msize_x + xx)->ground != build->ground  ||  (map + yy*msize_x + xx)->surface != empty)
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
            i++;
        }
    }

    printf("x: %zu, y: %zu\n", x, y);
    fseek(fmap, 0, SEEK_SET);

    fwrite(&msize_x, sizeof(size_t), 1, fmap);
    fflush(fmap);
    fwrite(map, msize_y * msize_x * sizeof(map_point_t), 1, fmap);
    fflush(fmap);
}

void buildgen()
{
    int buildid;
    char ch;
    char buf[255];
    size_t size_x, size_y;
    FILE *f;

    printf("buildid: ");
    while (scanf("%i", &buildid) == 0);

    printf("size_x: ");
    while (scanf("%zu", &size_x) == 0);

    printf("size_y: ");
    while (scanf("%zu", &size_y) == 0);

    map_point_t *build = malloc( size_y * size_x * sizeof(map_point_t) );

    printf("w - wall, f - floor\n");
    for (size_t y = 0; y < size_y; y++)
    {
        for (size_t x = 0; x < size_x; x++)
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
            (build + y*size_x + x)->name_id = buildid;
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

    sprintf(buf, "%i_%zu.build", buildid, size_x*size_y);

    f = fopen(buf, "wb");

    fwrite(&size_x, sizeof(size_t), 1, f);
    fflush(f);
    fwrite(&size_y, sizeof(size_t), 1, f);
    fflush(f);

    fwrite(build, size_y * size_x * sizeof(map_point_t), 1, f);
    fflush(f);

}