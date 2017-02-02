#include "gui_interface.h"
#include "gui.h"
#include "client.h"
#include "data_container.h"

#include <string.h>

extern init_win_t *init_win;
extern main_win_t *main_win;

static gboolean print_message_and_scroll(gpointer data)
{
    GtkTextIter iter;
    text_win_t *window = ((print_data*)data)->win;
    char* str = ((print_data*)data)->str;

    gtk_text_buffer_get_end_iter(window->buf, &iter);
    gtk_text_buffer_insert(window->buf, &iter, g_strconcat("\n", str, NULL), -1);

    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(main_win->main_text_win->text_view),
                                 gtk_text_buffer_get_insert(main_win->main_text_win->buf), 0.0, TRUE, 0.5, 0.5);

    g_free(data);
    return FALSE;
}

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
    size_t len;

    str = gtk_entry_get_text(entry);
    len = strlen(str);

    if(len < MIN_INPUT_LEN)
        return;

    /*+1 added to sending "\0"*/
    send_user_input(str, len + 1);

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

void gui_print_main_msg(char* str)
{
    print_data *data = print_data_new(main_win->main_text_win, str);
    if(data == NULL)
        return;

    gdk_threads_add_idle(print_message_and_scroll, data);
}

void gui_print_chat_msg(char* str)
{
    print_data *data = print_data_new(main_win->chat_win, str);
    if(data == NULL)
        return;

    gdk_threads_add_idle(print_message_and_scroll, data);
}

void gui_start(int argc, char* argv[])
{
    gtk_init(&argc, &argv);
    gui_init();

    gtk_widget_show_all(init_win->window);

    gtk_main();
}
