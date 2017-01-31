#ifndef _GUI_INTERFACE_H
#define _GUI_INTERFACE_H

#include <gtk/gtk.h>

void sp_command_enter(GtkEntry *entry);
void sp_destroy();
void sp_get_ip_window_enter(GtkWidget *widget, gpointer entry);

void start_gui(int argc, char* argv[]);

#endif