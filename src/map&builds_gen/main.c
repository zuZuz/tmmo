#include <stdio.h>
#include <string.h>
#include "../game_processing/map.h"
#include "buildgen.h"

#define COLOR_REGION_OFFSET 30

void map_to_file(FILE *fmap, size_t factor, size_t size_x, size_t size_y)
{
    printf("map_to_file\n");
    map_point_t *map = map_generation(factor, size_x, size_y, water, 10);
    size_t buildings_cnt = 0;

    fwrite(&buildings_cnt, sizeof(size_t), 1, fmap);
    fwrite(&size_x, sizeof(size_t), 1, fmap);
    fwrite(map, size_y * size_x * sizeof(map_point_t), 1, fmap);
    fflush(fmap);
}

void map_to_pic(FILE *fmap, FILE *fpic, FILE *fpic_reg)
{
    printf("map_to_pic\n");
    size_t size_x, size_y;
    size_t buildings_cnt;
    building_t *buildings;


    fread(&buildings_cnt, sizeof(size_t), 1, fmap);
    if(buildings_cnt != 0)
    {
        buildings = malloc( buildings_cnt * sizeof(building_t) );
        fread(buildings, buildings_cnt * sizeof(building_t), 1, fmap);

        for(int i = 0; i < buildings_cnt; i++)
        {
            printf("buildname_id: %i, coord: (%i, %i), size_x: %zu, size_y: %zu\n",
                   buildings[i].point.name_id, buildings[i].point.x, buildings[i].point.y, buildings[i].size_x, buildings->size_y);
        }
    }


    fread(&size_x, sizeof(size_t), 1, fmap);
    size_y = size_x;

    printf("x: %zu, y: %zu\n", size_x, size_y);

    map_point_t *bufmap = malloc( size_y * size_x * sizeof(map_point_t) );

    unsigned char *ptr;
    unsigned char *ptr_reg;
    unsigned char *bufpic = malloc(3 * size_x * size_y);
    unsigned char *bufpic_reg = malloc(3 * size_x * size_y);
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
    ptr_reg = bufpic_reg;

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

            if((bufmap + y * size_x + x)->ground < GROUND_TYPE_CNT)
            {
                for(int i = 0; i < 3; i++)
                    ptr_reg[i] = (bufmap + y * size_x + x)->region*(COLOR_REGION_OFFSET) + COLOR_REGION_OFFSET;
            }
            else
            {
                ptr_reg[0] = 0;
                ptr_reg[1] = 0;
                ptr_reg[2] = 0;
            }

            ptr += 3;
            ptr_reg += 3;
        }
    }

    fprintf(fpic, "P6\n%d %d\n255\n", (int)size_x, (int)size_y);
    fflush(fpic);
    fwrite(bufpic, size_y * size_x * 3, 1, fpic);
    fflush(fpic);

    fprintf(fpic_reg, "P6\n%d %d\n255\n", (int)size_x, (int)size_y);
    fflush(fpic_reg);
    fwrite(bufpic_reg, size_y * size_x * 3, 1, fpic_reg);
    fflush(fpic_reg);

}

int main(int argc, char* argv[])
{
    FILE *fmap, *fpic, *fpic_reg, *fbuild;
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
            printf("can't r/w to %s.map\n", str);
            return 1;
        }

        if (argv[2][0] == '-')
        {
            strcpy(str, argv[2] + 1);
            if(strlen(str) == 0)
                return 1;

            fpic = fopen(strcat(str, ".pnm"), "w");
            if (fpic == NULL) {
                printf("can't write to %s.pnm\n", str);
                return 1;
            }

            strcpy(str, argv[2] + 1);
            strcat(str, "_regions");
            fpic_reg = fopen(strcat(str, ".pnm"), "w");
            if (fpic_reg == NULL) {
                printf("can't write to %s.pnm\n", str);
                return 1;
            }

            map_to_pic(fmap, fpic, fpic_reg);
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
            printf("can't r/w to %s.map\n", str);
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