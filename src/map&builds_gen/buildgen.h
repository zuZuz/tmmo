#include "../game_processing/map.h"

#ifndef BUILDGEN_H
#define BUILDGEN_H

#pragma pack(push, 1)
typedef struct building
{
    map_point_t point;
    size_t size_x;
    size_t size_y;
} building_t;
#pragma pack(pop)


void buildgen();
void add_on_map(FILE *fmap, FILE *fbuild);

#endif //BUILDGEN_H
