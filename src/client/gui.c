#include <gtk/gtk.h>
#include "gui_interface.h"
#include "gui.h"

GtkWidget *main_window, *get_ip_window,
            *text_view_main;

GtkTextBuffer *text_main_buffer, *text_chat_buffer, *text_location_buffer;

static GtkWidget *entry_get_ip, *label_get_ip, *button_get_ip, *vbox_get_ip,
                 *scrolled_window_main, *scrolled_window_chat, *scrolled_window_location,
                 *entry_command_line, *text_view_chat, *text_view_location,
                 *main_grid;

static void init_get_ip_window()
{
    /*initialize widgets*/
    get_ip_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    entry_get_ip = gtk_entry_new();
    label_get_ip = gtk_label_new("Enter server address\n\taddress:port");
    button_get_ip = gtk_button_new_with_label("Connect");
    vbox_get_ip = gtk_box_new(GTK_ORIENTATION_VERTICAL, INIT_WINDOW_SPACING);

    /*setting get_ip window*/
    gtk_window_set_title(GTK_WINDOW(get_ip_window), "Enter IP window");
    gtk_container_set_border_width(GTK_CONTAINER(get_ip_window), WINDOW_BORDER_SIZE);
    gtk_window_set_resizable(GTK_WINDOW(get_ip_window), FALSE);
    gtk_widget_set_size_request(get_ip_window, INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT);

    /*packing widgets*/
    gtk_box_pack_start(GTK_BOX(vbox_get_ip), label_get_ip, TRUE, TRUE, INIT_WINDOW_SPACING);
    gtk_box_pack_start(GTK_BOX(vbox_get_ip), entry_get_ip, FALSE, FALSE, INIT_WINDOW_SPACING);
    gtk_box_pack_start(GTK_BOX(vbox_get_ip), button_get_ip, FALSE, FALSE, INIT_WINDOW_SPACING);

    /*adding to get_ip_window*/
    gtk_container_add(GTK_CONTAINER(get_ip_window), vbox_get_ip);
}

static void init_main_window()
{
    /*initialize widgets*/
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    scrolled_window_main = gtk_scrolled_window_new(NULL, NULL);
    scrolled_window_chat = gtk_scrolled_window_new(NULL, NULL);
    scrolled_window_location = gtk_scrolled_window_new(NULL, NULL);
    main_grid = gtk_grid_new();
    entry_command_line = gtk_entry_new();
    text_main_buffer = gtk_text_buffer_new(NULL);
    text_chat_buffer = gtk_text_buffer_new(NULL);
    text_location_buffer = gtk_text_buffer_new(NULL);
    text_view_main = gtk_text_view_new_with_buffer(text_main_buffer);
    text_view_chat = gtk_text_view_new_with_buffer(text_chat_buffer);
    text_view_location = gtk_text_view_new_with_buffer(text_location_buffer);

    /*setting command entry*/
    gtk_entry_set_max_length(GTK_ENTRY(entry_command_line), MAX_INPUT_LEN);

    /*setting main window*/
    gtk_window_set_title(GTK_WINDOW(main_window), "Main window");
    gtk_container_set_border_width(GTK_CONTAINER(main_window), WINDOW_BORDER_SIZE);

    /*setting main text window*/
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view_main), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view_main), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view_main), GTK_WRAP_WORD_CHAR);

    g_object_set(scrolled_window_main, "expand", TRUE, NULL);
    gtk_widget_set_size_request(scrolled_window_main, DEFAULT_CENTER_WIDTH, DEFAULT_CENTER_TEXT_HEIGHT);
    gtk_container_add(GTK_CONTAINER(scrolled_window_main), text_view_main);

    /*setting chat text window*/
    gtk_widget_set_sensitive(text_view_chat, FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view_chat), GTK_WRAP_WORD_CHAR);

    g_object_set(scrolled_window_chat, "expand", TRUE, NULL);
    gtk_widget_set_size_request(scrolled_window_chat, DEFAULT_COLUMN_WIDTH, -1);
    gtk_container_add(GTK_CONTAINER(scrolled_window_chat), text_view_chat);

    gtk_text_buffer_set_text(text_chat_buffer, "Chat window", -1);

    /*setting location text window*/
    gtk_widget_set_sensitive(text_view_location, FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view_location), GTK_WRAP_WORD_CHAR);

    g_object_set(scrolled_window_location, "expand", TRUE, NULL);
    gtk_widget_set_size_request(scrolled_window_location, DEFAULT_COLUMN_WIDTH, -1);
    gtk_container_add(GTK_CONTAINER(scrolled_window_location), text_view_location);

    gtk_text_buffer_set_text(text_location_buffer, "Location description window", -1);

    /*setting main_grid*/
    gtk_grid_set_column_spacing(GTK_GRID(main_grid), GRID_COLUMN_SPACING);
    gtk_grid_set_row_spacing(GTK_GRID(main_grid), GRID_ROW_SPACING);

    gtk_grid_attach(GTK_GRID(main_grid), scrolled_window_main, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), entry_command_line, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), scrolled_window_chat, 0, 0, 1, 2);
    gtk_grid_attach(GTK_GRID(main_grid), scrolled_window_location, 2, 0, 1, 2);

    /*adding grid to main_window*/
    gtk_container_add(GTK_CONTAINER(main_window), main_grid);
}

static void connect_signals()
{

    /*connect for main_window*/
    g_signal_connect(main_window, "destroy", G_CALLBACK(sp_destroy), NULL);
    g_signal_connect(entry_command_line, "activate", G_CALLBACK(sp_command_enter), NULL);

    /*connect for get_ip_window*/
    g_signal_connect(get_ip_window, "destroy", G_CALLBACK(sp_destroy), NULL);
    g_signal_connect(button_get_ip, "clicked", G_CALLBACK(sp_get_ip_window_enter), (gpointer)entry_get_ip);
    g_signal_connect(entry_get_ip, "activate", G_CALLBACK(sp_get_ip_window_enter), (gpointer)entry_get_ip);
}

void gui_init()
{
    init_main_window();
    init_get_ip_window();

    connect_signals();
}
