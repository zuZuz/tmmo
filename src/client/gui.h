#ifndef _GUI_H
#define _GUI_H

#include <gtk/gtk.h>
#include "map.h"

#define MIN_INPUT_LEN 3

typedef struct text_win_t {
    GtkTextBuffer *buf;
    GtkWidget *scroll_win, *text_view;
}text_win_t;

typedef struct init_win_t {
    GtkWidget *window, *entry, *button;
}init_win_t;

typedef struct main_win_t {
    GtkWidget *window, *comm_entry,
                *nick_lbl, *hp_lbl, *atp_lbl,
                *arm_lbl, *evn_lbl, *lvl_lbl;
    text_win_t *main_text_win, *chat_win, *online_win;
    map_t *map;
}main_win_t;

typedef struct char_info {
    char    nick[16],
            lvl[16],
            hp[16],
            atp[16],
            arm[16],
            evn[16];
}char_info_t;

/*Data wrappers to modify gui from other threads*/
typedef struct print_data_wrapper{
    void *element;
    char *str;
}print_data_wrapper_t;


void gui_print_main_msg(char* str);
void gui_print_chat_msg(char* str);
void gui_print_online_list(char* online_list);
void gui_map_update(map_point_t* points);
void gui_print_char_info(char_info_t* char_info);

int gui_start(int argc, char** argv);

#endif
