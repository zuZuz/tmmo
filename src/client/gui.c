#include "gui.h"
#include "client.h"

#include <string.h>
#include <stdlib.h>

static init_win_t *init_win;
static main_win_t *main_win;

static print_data_wrapper_t* print_data_wrapper_create(void *element, char *str);
static void sp_send_prepared_input(GtkWidget *widget, gpointer str);

static void sp_command_enter(GtkEntry *entry);
static void sp_ip_enter(GtkWidget *widget, gpointer entry);
static void free_mem();

static void activate(GtkApplication* app, gpointer user_data)
{
    GtkBuilder  *builder;

    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    GtkWidget *up_but, *down_but, *right_but, *left_but;

    builder = gtk_builder_new_from_file("src/client/gui/main.glade");

    main_win = g_malloc(sizeof(main_win_t));
    main_win->map = g_malloc(sizeof(map_t));
    main_win->main_text_win = g_malloc(sizeof(text_win_t));
    main_win->chat_win = g_malloc(sizeof(text_win_t));
    main_win->online_win = g_malloc(sizeof(text_win_t));

    init_win = g_malloc(sizeof(init_win_t));

    up_but = GTK_WIDGET(gtk_builder_get_object(builder, "up_but"));
    down_but = GTK_WIDGET(gtk_builder_get_object(builder, "down_but"));
    right_but = GTK_WIDGET(gtk_builder_get_object(builder, "right_but"));
    left_but = GTK_WIDGET(gtk_builder_get_object(builder, "left_but"));

    main_win->window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    main_win->comm_entry = GTK_WIDGET(gtk_builder_get_object(builder, "comm_entry"));
    main_win->map->draw_area = GTK_WIDGET(gtk_builder_get_object(builder, "map_area"));
    main_win->main_text_win->buf = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "main_text_buffer"));
    main_win->main_text_win->scroll_win = GTK_WIDGET(gtk_builder_get_object(builder, "main_scrolledwin"));
    main_win->main_text_win->text_view = GTK_WIDGET(gtk_builder_get_object(builder, "main_text"));

    main_win->chat_win->buf = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "chat_text_buffer"));
    main_win->chat_win->scroll_win = GTK_WIDGET(gtk_builder_get_object(builder, "chat_window"));
    main_win->chat_win->text_view = GTK_WIDGET(gtk_builder_get_object(builder, "chat_text"));

    main_win->online_win->buf = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "online_text_buffer"));
    main_win->online_win->scroll_win = GTK_WIDGET(gtk_builder_get_object(builder, "online_window"));
    main_win->online_win->text_view = GTK_WIDGET(gtk_builder_get_object(builder, "online_text"));

    main_win->nick_lbl = GTK_WIDGET(gtk_builder_get_object(builder, "nick_lbl"));
    main_win->lvl_lbl = GTK_WIDGET(gtk_builder_get_object(builder, "lvl_lbl"));
    main_win->hp_lbl = GTK_WIDGET(gtk_builder_get_object(builder, "hp_lbl"));
    main_win->atp_lbl = GTK_WIDGET(gtk_builder_get_object(builder, "atp_lbl"));
    main_win->arm_lbl = GTK_WIDGET(gtk_builder_get_object(builder, "arm_lbl"));
    main_win->evn_lbl = GTK_WIDGET(gtk_builder_get_object(builder, "evn_lbl"));

    init_win->window = GTK_WIDGET(gtk_builder_get_object(builder, "get_server_window"));
    init_win->entry = GTK_WIDGET(gtk_builder_get_object(builder, "address_entry"));
    init_win->button = GTK_WIDGET(gtk_builder_get_object(builder, "connect_button"));

    /*connect buttons*/
    g_signal_connect(up_but, "clicked", G_CALLBACK(sp_send_prepared_input), PREP_GO_UP);
    g_signal_connect(down_but, "clicked", G_CALLBACK(sp_send_prepared_input), PREP_GO_DOWN);
    g_signal_connect(right_but, "clicked", G_CALLBACK(sp_send_prepared_input), PREP_GO_RIGHT);
    g_signal_connect(left_but, "clicked", G_CALLBACK(sp_send_prepared_input), PREP_GO_LEFT);

    /*connect for main_win->window*/
    g_signal_connect(main_win->comm_entry, "activate", G_CALLBACK(sp_command_enter), NULL);
    g_signal_connect (main_win->map->draw_area,"configure-event", G_CALLBACK (sp_draw_area_init), main_win->map);
    g_signal_connect (main_win->map->draw_area, "draw", G_CALLBACK (sp_draw), main_win->map);

    /*connect for init_win->window*/
    g_signal_connect(init_win->button, "clicked", G_CALLBACK(sp_ip_enter), (gpointer)init_win->entry);
    g_signal_connect(init_win->entry, "activate", G_CALLBACK(sp_ip_enter), (gpointer)init_win->entry);

    gtk_application_add_window(app, GTK_WINDOW(init_win->window));

    /*For CSS*/
    provider = gtk_css_provider_new ();
    display = gdk_display_get_default ();
    screen = gdk_display_get_default_screen (display);
    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GError *error = 0;
    gtk_css_provider_load_from_file(provider, g_file_new_for_path("src/client/gui/style.css"), &error);
    g_object_unref (provider);


    gtk_widget_show_all(init_win->window);
}

int gui_start(int argc, char* argv[])
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.test.tmmo", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}

void free_mem()
{
    g_free(main_win->main_text_win);
    g_free(main_win->chat_win);
    g_free(main_win->online_win);
    g_free(main_win->map);

    g_free(main_win);
    g_free(init_win);
}

static gboolean print_char_info(gpointer data)
{
    gtk_label_set_text(GTK_LABEL(main_win->nick_lbl), ((char_info_t*)data)->nick);
    gtk_label_set_text(GTK_LABEL(main_win->lvl_lbl), ((char_info_t*)data)->lvl);
    gtk_label_set_text(GTK_LABEL(main_win->hp_lbl), ((char_info_t*)data)->hp);
    gtk_label_set_text(GTK_LABEL(main_win->atp_lbl), ((char_info_t*)data)->atp);
    gtk_label_set_text(GTK_LABEL(main_win->arm_lbl), ((char_info_t*)data)->arm);
    gtk_label_set_text(GTK_LABEL(main_win->evn_lbl), ((char_info_t*)data)->evn);

    return FALSE;
}

static gboolean print_message_and_scroll(gpointer data)
{
    GtkTextIter iter;
    text_win_t *window = ((print_data_wrapper_t*)data)->element;
    char* str = ((print_data_wrapper_t*)data)->str;

    gtk_text_buffer_get_end_iter(window->buf, &iter);
    gtk_text_buffer_insert(window->buf, &iter, g_strconcat("\n", str, NULL), -1);

    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(main_win->main_text_win->text_view),
                                 gtk_text_buffer_get_insert(main_win->main_text_win->buf), 0.0, true, 0.5, 0.5);

    g_free(data);
    return FALSE;
}

static gboolean print_online_list(gpointer data)
{
    gtk_text_buffer_set_text(main_win->online_win->buf, (char*)data, -1);

    g_free(data);
    return FALSE;
}

static bool check_ip(const gchar* addr)
{
    char **addr_split, *serv_ip;
    unsigned short serv_port;

    guint64 parce_port;

    if(!addr) return false;

    addr_split = g_strsplit(addr, ":", 2);

    if(g_strv_length(addr_split) != 2) return false;

    parce_port = g_ascii_strtoull(addr_split[1], NULL, 0);
    if(parce_port == 0 || parce_port > USHRT_MAX) return false;

    serv_ip = addr_split[0];
    serv_port = (unsigned short) parce_port;

    if(!connect_to_serv(serv_ip, serv_port)) return false;

    g_free(addr_split);
    return true;
}

static void sp_send_prepared_input(GtkWidget *widget, gpointer str)
{
    /*+1 added to send "\0"*/
    send_user_input(str, strlen((char*)str) + 1);
}

static void sp_ip_enter(GtkWidget *widget, gpointer entry)
{
    if(!check_ip(gtk_entry_get_text(GTK_ENTRY(entry)))) return;

    gtk_application_add_window(gtk_window_get_application(GTK_WINDOW(init_win->window)), GTK_WINDOW(main_win->window));

    gtk_widget_destroy(init_win->window);
    gtk_widget_show_all(main_win->window);
}

static void sp_command_enter(GtkEntry *entry)
{
    const char* str;
    size_t len;

    str = gtk_entry_get_text(entry);
    len = strlen(str);

    if(len < MIN_INPUT_LEN) return;

    /*+1 added to send "\0"*/
    send_user_input(str, len + 1);

    gtk_editable_delete_text(GTK_EDITABLE(entry), 0, -1);
}

void gui_print_main_msg(char* str)
{
    print_data_wrapper_t *data = print_data_wrapper_create(main_win->main_text_win, str);
    if(data == NULL) return;

    g_idle_add(print_message_and_scroll, data);
}

void gui_print_chat_msg(char* str)
{
    print_data_wrapper_t *data = print_data_wrapper_create(main_win->chat_win, str);
    if(data == NULL) return;

    g_idle_add(print_message_and_scroll, data);
}

void gui_map_update(map_point_t* points)
{
    map_load(points, main_win->map);

    memcpy(main_win->map->pos, (void*)points + sizeof(map_point_t)*MAP_SCALE*MAP_SCALE, sizeof(char)*16);

    g_idle_add(map_refresh, main_win->map);
}

void gui_print_char_info(char_info_t* char_info)
{
    if(char_info == NULL) return;

    g_idle_add(print_char_info, char_info);
}

void gui_print_online_list(char* online_list)
{
    g_idle_add(print_online_list, online_list);
}

static print_data_wrapper_t* print_data_wrapper_create(void *element, char *str)
{
    print_data_wrapper_t *data = g_malloc(sizeof(print_data_wrapper_t));

    if(data == NULL) return NULL;

    data->element = element;
    data->str = str;

    return data;
}