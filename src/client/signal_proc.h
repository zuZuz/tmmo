#ifndef _SIGNAL_PROC_H
#define _SIGNAL_PROC_H

void sp_command_enter(GtkEntry *entry, gpointer buffer);
void sp_destroy();
void sp_check_ip(GtkWidget *widget, gpointer window);
void sp_show_window(GtkWidget *widget, gpointer show_window);

#endif