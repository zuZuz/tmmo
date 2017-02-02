#include "gui_interface.h"
#include "gui.h"
#include "client.h"

#include <string.h>

extern init_win_t *init_win;
extern main_win_t *main_win;

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
    GtkTextIter iter;

    gtk_text_buffer_get_end_iter(buffer, &iter);
    gtk_text_buffer_insert(buffer, &iter, g_strconcat("\n", text, NULL), len + 1);
}

void sp_ip_enter(GtkWidget *widget, gpointer entry)
{

    if(!check_ip(gtk_entry_get_text(GTK_ENTRY(entry))))
        return;

    gtk_widget_hide(init_win->window);

    gtk_widget_show_all(main_win->window);
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

void sp_destroy_main_window()
{
    close_connection();
    gtk_main_quit();

    free_mem();
}

void sp_destroy_init_window()
{
    gtk_main_quit();

    free_mem();
}

void gui_print_main_msg(char* str, int len)
{
    buffer_print_new_line(main_win->main_text_win->buf, str, len);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(main_win->main_text_win->text_view),
                                 gtk_text_buffer_get_insert(main_win->main_text_win->buf), 0.0, TRUE, 0.5, 0.5);
}

void gui_print_chat_msg(char* str, int len)
{
    buffer_print_new_line(main_win->chat_win->buf, str, len);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(main_win->chat_win->text_view),
                                 gtk_text_buffer_get_insert(main_win->chat_win->buf), 0.0, TRUE, 0.5, 0.5);
}

void gui_start(int argc, char* argv[])
{
    gtk_init(&argc, &argv);
    gui_init();

    gtk_widget_show_all(init_win->window);

    gtk_main();
}
