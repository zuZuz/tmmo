#include "data_container.h"

print_data* print_data_new(text_win_t* win, char *str)
{
    print_data *data = g_new(struct print_data, 1);

    if(data == NULL)
        return NULL;

    data->win = win;
    data->str = str;

    return data;
}
