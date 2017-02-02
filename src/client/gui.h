#ifndef _GUI_H
#define _GUI_H

#define DEFAULT_CENTER_WIDTH 300
#define DEFAULT_CENTER_TEXT_HEIGHT 300
#define DEFAULT_COLUMN_WIDTH 200
#define GRID_COLUMN_SPACING 5
#define GRID_ROW_SPACING 2
#define WINDOW_BORDER_SIZE 5

#define INIT_WINDOW_WIDTH 250
#define INIT_WINDOW_HEIGHT 200
#define INIT_WINDOW_SPACING 2

#define MAX_INPUT_LEN 1024

#include <gtk/gtk.h>

typedef struct text_win_t {
    GtkTextBuffer *buf;
    GtkWidget *scroll_win, *text_view;
}text_win_t;

typedef struct init_win_t {
    GtkWidget *window, *entry, *button, *label,
                *vbox;
}init_win_t;

typedef struct main_win_t{
    GtkWidget *window, *grid, *comm_entry;
    text_win_t *main_text_win, *chat_win, *loc_discr_win;
}main_win_t;


void gui_init();
void free_mem();

#endif
