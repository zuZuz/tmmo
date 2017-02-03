#include <stdio.h>
#include <string.h>
#include "../map.h"
#include "buildgen.h"

void map_to_file(FILE *fmap, size_t factor, size_t size_x, size_t size_y)
{
    printf("map_to_file\n");
    map_point_t *map = map_generation(factor, size_x, size_y, water, 10);

    fwrite(&size_x, sizeof(size_t), 1, fmap);
    fflush(fmap);
    fwrite(map, size_y * size_x * sizeof(map_point_t), 1, fmap);
    fflush(fmap);
}

void map_to_pic(FILE *fmap, FILE *fpic)
{
    printf("map_to_pic\n");
    size_t size_x, size_y;

    fread(&size_x, sizeof(size_t), 1, fmap);
    size_y = size_x;

    printf("x: %i, y: %i\n", size_x, size_y);

    map_point_t *bufmap = malloc( size_y * size_x * sizeof(map_point_t) );

    unsigned char *ptr;
    unsigned char *bufpic = malloc(3 * size_x * size_y);
    unsigned char rgb[GROUND_TYPE_CNT + 1][3] =
            {
                    {165, 42, 42},
                    {255, 255, 0},
                    {0, 255, 0},
                    {0, 0, 255}
            };
    unsigned char rgb_surfs[SURFACE_TYPE_CNT][3] =
            {
                    {0, 0, 0},
                    {255, 255, 255},
            };

    ptr = bufpic;

    fread(bufmap, size_x * size_y * sizeof(map_point_t), 1, fmap);


    for (size_t y = 0; y < size_y; y++) {
        for (size_t x = 0; x < size_x; x++)
        {
            if((bufmap + y * size_x + x)->surface == empty)
            {
                memcpy(ptr, rgb[(bufmap + y * size_x + x)->ground], 3);
            }
            else
            {
                memcpy(ptr, rgb_surfs[(bufmap + y * size_x + x)->surface - 1], 3);
            }

            ptr += 3;
        }
    }

    fprintf(fpic, "P6\n%d %d\n255\n", (int)size_x, (int)size_y);
    fflush(fpic);
    fwrite(bufpic, size_y * size_x * 3, 1, fpic);
    fflush(fpic);

}

int main(int argc, char* argv[])
{
    FILE *fmap, *fpic, *fbuild;
    size_t factor, size_x, size_y;
    char str[255];

    if (argc < 2)
    {
        buildgen();
        return 1;
    }

    if (argc > 2)
    {

        strcpy(str, argv[1]);
        fmap = fopen(strcat(str, ".map"), "rb+");
        if (fmap == NULL) {
            printf("can't r/w to %s.map\n", argv[1]);
            return 1;
        }

        if (argv[2][0] == '-')
        {
            strcpy(str, argv[2] + 1);
            if(strlen(str) == 0)
                return 1;

            fpic = fopen(strcat(str, ".pnm"), "w");
            if (fpic == NULL) {
                printf("can't write to %s.pnm\n", argv[2]);
                return 1;
            }

            map_to_pic(fmap, fpic);
        }
        else
        {

            for(int i = 2; i < argc; i++)
            {
                strcpy(str, argv[2]);
                strtok(str, ".");
                if(strcmp( strtok(NULL, "."), "build" ) == 0)
                {
                    fbuild = fopen(argv[2], "rb");
                    if(fbuild == NULL)
                        continue;

                    add_on_map(fmap, fbuild);
                    fclose(fbuild);
                }
            }
        }

    }
    else
    {
        strcpy(str, argv[1]);
        fmap = fopen(strcat(str, ".map"), "wb+");
        if (fmap == NULL) {
            printf("can't r/w to %s.map\n", argv[1]);
            return 1;
        }

        printf("factor: ");
        while (scanf("%zu", &factor) == 0);

        printf("size: ");
        while (scanf("%zu", &size_x) == 0);

        size_y = size_x;

        map_to_file(fmap, factor, size_x, size_y);
    }


    return 0;
}