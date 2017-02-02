#ifndef _GUI_INTERFACE_H
#define _GUI_INTERFACE_H

#include <gtk/gtk.h>

#define MIN_INPUT_LEN 3

void sp_command_enter(GtkEntry *entry);
void sp_destroy_main_window();
void sp_destroy_init_window();
void sp_ip_enter(GtkWidget *widget, gpointer entry);

void gui_print_main_msg(char* str);
void gui_print_chat_msg(char* str);
void gui_start(int argc, char* argv[]);

#endif