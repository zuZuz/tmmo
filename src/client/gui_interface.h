#ifndef _GUI_INTERFACE_H
#define _GUI_INTERFACE_H

#include <gtk/gtk.h>

#define MIN_INPUT_LEN 3

void sp_command_enter(GtkEntry *entry);
void sp_destroy();
void sp_get_ip_window_enter(GtkWidget *widget, gpointer entry);

void gui_print_main_msg(char* str, int len);
void gui_print_chat_msg(char* str, int len);
void gui_start(int argc, char* argv[]);

#endif