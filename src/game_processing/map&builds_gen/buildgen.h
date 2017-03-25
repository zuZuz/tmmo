#ifndef BUILDGEN_H
#define BUILDGEN_H

typedef struct building
{
    map_point_t point;
    size_t size_x;
    size_t size_y;
} building_t;


void buildgen();
void add_on_map(FILE *fmap, FILE *fbuild);

#endif //BUILDGEN_H
