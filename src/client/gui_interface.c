#include "gui_interface.h"
#include "gui.h"
#include "client.h"

#include <string.h>

extern GtkWidget *main_window, *get_ip_window,
                    *text_view_main;

extern GtkTextBuffer *text_main_buffer, *text_chat_buffer, *text_location_buffer;

static bool check_ip(const gchar* addr)
{
    char **addr_split, *serv_ip;
    unsigned short serv_port;

    guint64 parce_port;

    if(!addr)
        return false;

    addr_split = g_strsplit(addr, ":", 2);

    if(g_strv_length(addr_split) != 2)
        return false;

    parce_port = g_ascii_strtoull(addr_split[1], NULL, 0);
    if(parce_port == 0 || parce_port > USHRT_MAX)
        return false;

    serv_ip = addr_split[0];
    serv_port = (unsigned short) parce_port;

    if(!connect_to_serv(serv_ip, serv_port))
        return false;

    g_free(addr_split);

    return true;
}

static void buffer_print_new_line(GtkTextBuffer* buffer, char* text, int len)
{
    if(gtk_text_buffer_get_char_count(buffer))
    {
        gtk_text_buffer_insert_at_cursor(buffer, "\n", 1);
    }

    gtk_text_buffer_insert_at_cursor(buffer, text, len);
}

void sp_get_ip_window_enter(GtkWidget *widget, gpointer entry)
{

    if(!check_ip(gtk_entry_get_text(GTK_ENTRY(entry))))
        return;

    gtk_widget_hide(get_ip_window);

    gtk_widget_show_all(main_window);
}

void sp_command_enter(GtkEntry *entry)
{
    const char* str;
    str = gtk_entry_get_text(entry);

    if(strlen(str) < MIN_INPUT_LEN)
        return;

    send_user_input(str);

    gtk_editable_delete_text(GTK_EDITABLE(entry), 0, -1);
}

void sp_destroy()
{
    close_connection();
    gtk_main_quit();
}

void gui_print_main_msg(char* str, int len)
{
    buffer_print_new_line(text_main_buffer, str, len);
}

void gui_print_chat_msg(char* str, int len)
{
    buffer_print_new_line(text_chat_buffer, str, len);
}

void gui_start(int argc, char* argv[])
{
    gtk_init(&argc, &argv);
    gui_init();

    gtk_widget_show_all(get_ip_window);

    gtk_main();
}
