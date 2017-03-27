#include <stdlib.h>
#include <stdio.h>

#ifndef MAP_H
#define MAP_H

#define GROUND_TYPE_CNT 3
typedef enum ground_type
{
    soil = 0,
    sand,
    grass,
    water
} ground_type_t;

typedef enum object_type
{   nothing,
    player,
    enemy
} object_type_t;

#define SURFACE_TYPE_CNT 2
typedef enum surface_type
{
    empty,
    wall,
    floor
} surface_type_t;

#pragma pack(push, 1)
typedef struct map_point
{
    int x;
    int y;
    ground_type_t ground;
    surface_type_t surface;
    object_type_t child_object_type;
    void *child_object;
    int name_id;
    size_t region;
} map_point_t;
#pragma pack(pop)

map_point_t* map_generation(size_t factor, size_t size_x, size_t size_y, ground_type_t bound, size_t indent);

#endif //MAP_H
