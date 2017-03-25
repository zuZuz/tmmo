#ifndef _GUI_H
#define _GUI_H

#include <gtk/gtk.h>

#define MIN_INPUT_LEN 3

typedef struct text_win_t {
    GtkTextBuffer *buf;
    GtkWidget *scroll_win, *text_view;
}text_win_t;

typedef struct init_win_t {
    GtkWidget *window, *entry, *button;
}init_win_t;

typedef struct main_win_t{
    GtkWidget *window, *comm_entry;
    text_win_t *main_text_win, *chat_win, *online_win;
}main_win_t;

void gui_print_main_msg(char* str);
void gui_print_chat_msg(char* str);

int gui_start(int argc, char** argv);

void free_mem();

#endif
