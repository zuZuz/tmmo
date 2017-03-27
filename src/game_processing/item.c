#include "item.h"

item_t* item_new(item_type_t type, char *name, characteristics_t characteristics, int strength)
{
    item_t *new_item = malloc(sizeof(item_t));

    new_item->type = type;
    new_item->name = name;
    new_item->characteristics = characteristics;
    new_item->strength = strength;

    return new_item;
}