#include "character.h"
#include "../map&builds_gen/buildgen.h"

#ifndef ITEM_H
#define ITEM_H

typedef struct item_building
{
    building_t *building;
    items_t *items;
} item_building_t;

item_t* item_new(item_type_t type, char *name, characteristics_t characteristics, int strength);

#endif //ITEM_H
