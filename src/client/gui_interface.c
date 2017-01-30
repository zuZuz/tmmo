#include <gtk/gtk.h>
#include "client.h"
#include "signal_proc.h"

void sp_command_enter(GtkEntry *entry, gpointer buffer)
{

}

void sp_destroy()
{
    gtk_main_quit();
}

void sp_show_window(GtkWidget *widget, gpointer show_window)
{
    gtk_widget_show_all((GtkWidget*)(show_window));
}

void sp_check_ip(GtkWidget *widget, gpointer entry)
{
    const gchar *addr;
    char **addr_split, *serv_ip;

    guint64 parce_port;
    unsigned short serv_port;

    addr = gtk_entry_get_text(GTK_ENTRY(entry));
    if(!addr)
        return;

    addr_split = g_strsplit(addr, ":", 2);

    if(g_strv_length(addr_split) != 2)
        return;

    parce_port = g_ascii_strtoull(addr_split[1], NULL, 0);
    if(parce_port == 0 || parce_port > USHRT_MAX)
        return;

    serv_ip = addr_split[0];
    serv_port = (unsigned short) parce_port;

    if(!connect_to_serv(serv_ip, serv_port))
        return;

    gtk_widget_hide(gtk_widget_get_toplevel(widget));
}
