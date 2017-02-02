#ifndef DATA_CONTAINER_H
#define DATA_CONTAINER_H

#include "gui.h"

typedef struct print_data{
    text_win_t *win;
    char *str;
}print_data;

print_data* print_data_new(text_win_t* win, char *str);

#endif
