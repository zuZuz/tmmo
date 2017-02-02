#include "gui_interface.h"
#include "gui.h"

init_win_t *init_win;
main_win_t *main_win;

static void init_init_window()
{
    /*initialize widgets*/
    init_win->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    init_win->entry = gtk_entry_new();
    init_win->label = gtk_label_new("Enter server address\n\taddress:port");
    init_win->button = gtk_button_new_with_label("Connect");
    init_win->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, INIT_WINDOW_SPACING);

    /*setting get_ip window*/
    gtk_window_set_title(GTK_WINDOW(init_win->window), "Enter IP window");
    gtk_window_set_position(GTK_WINDOW(init_win->window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(init_win->window), WINDOW_BORDER_SIZE);
    gtk_window_set_resizable(GTK_WINDOW(init_win->window), FALSE);
    gtk_widget_set_size_request(init_win->window, INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT);

    /*packing widgets*/
    gtk_box_pack_start(GTK_BOX(init_win->vbox), init_win->label, TRUE, TRUE, INIT_WINDOW_SPACING);
    gtk_box_pack_start(GTK_BOX(init_win->vbox), init_win->entry, FALSE, FALSE, INIT_WINDOW_SPACING);
    gtk_box_pack_start(GTK_BOX(init_win->vbox), init_win->button, FALSE, FALSE, INIT_WINDOW_SPACING);

    /*adding to init_win->window*/
    gtk_container_add(GTK_CONTAINER(init_win->window), init_win->vbox);
}

static void init_main_window()
{
    /*initialize widgets*/
    main_win->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    main_win->main_text_win->scroll_win = gtk_scrolled_window_new(NULL, NULL);
    main_win->chat_win->scroll_win = gtk_scrolled_window_new(NULL, NULL);
    main_win->loc_discr_win->scroll_win = gtk_scrolled_window_new(NULL, NULL);
    main_win->grid = gtk_grid_new();
    main_win->comm_entry = gtk_entry_new();
    main_win->main_text_win->buf = gtk_text_buffer_new(NULL);
    main_win->chat_win->buf = gtk_text_buffer_new(NULL);
    main_win->loc_discr_win->buf = gtk_text_buffer_new(NULL);
    main_win->main_text_win->text_view = gtk_text_view_new_with_buffer(main_win->main_text_win->buf);
    main_win->chat_win->text_view = gtk_text_view_new_with_buffer(main_win->chat_win->buf);
    main_win->loc_discr_win->text_view = gtk_text_view_new_with_buffer(main_win->loc_discr_win->buf);

    /*setting command entry*/
    gtk_entry_set_max_length(GTK_ENTRY(main_win->comm_entry), MAX_INPUT_LEN);

    /*setting main window*/
    gtk_window_set_title(GTK_WINDOW(main_win->window), "Main window");
    gtk_window_set_position(GTK_WINDOW(main_win->window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(main_win->window), WINDOW_BORDER_SIZE);

    /*setting main text window*/
    gtk_text_view_set_editable(GTK_TEXT_VIEW(main_win->main_text_win->text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(main_win->main_text_win->text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(main_win->main_text_win->text_view), GTK_WRAP_WORD_CHAR);

    g_object_set(main_win->main_text_win->scroll_win, "expand", TRUE, NULL);
    gtk_widget_set_size_request(main_win->main_text_win->scroll_win, DEFAULT_CENTER_WIDTH, DEFAULT_CENTER_TEXT_HEIGHT);
    gtk_container_add(GTK_CONTAINER(main_win->main_text_win->scroll_win), main_win->main_text_win->text_view);

    /*setting chat text window*/
    gtk_widget_set_sensitive(main_win->chat_win->text_view, FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(main_win->chat_win->text_view), GTK_WRAP_WORD_CHAR);

    g_object_set(main_win->chat_win->scroll_win, "expand", TRUE, NULL);
    gtk_widget_set_size_request(main_win->chat_win->scroll_win, DEFAULT_COLUMN_WIDTH, -1);
    gtk_container_add(GTK_CONTAINER(main_win->chat_win->scroll_win), main_win->chat_win->text_view);

    gtk_text_buffer_set_text(main_win->chat_win->buf, "Chat window", -1);

    /*setting location text window*/
    gtk_widget_set_sensitive(main_win->loc_discr_win->text_view, FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(main_win->loc_discr_win->text_view), GTK_WRAP_WORD_CHAR);

    g_object_set(main_win->loc_discr_win->scroll_win, "expand", TRUE, NULL);
    gtk_widget_set_size_request(main_win->loc_discr_win->scroll_win, DEFAULT_COLUMN_WIDTH, -1);
    gtk_container_add(GTK_CONTAINER(main_win->loc_discr_win->scroll_win), main_win->loc_discr_win->text_view);

    gtk_text_buffer_set_text(main_win->loc_discr_win->buf, "Location description window", -1);

    /*setting main_win->grid*/
    gtk_grid_set_column_spacing(GTK_GRID(main_win->grid), GRID_COLUMN_SPACING);
    gtk_grid_set_row_spacing(GTK_GRID(main_win->grid), GRID_ROW_SPACING);

    gtk_grid_attach(GTK_GRID(main_win->grid), main_win->main_text_win->scroll_win, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(main_win->grid), main_win->comm_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(main_win->grid), main_win->chat_win->scroll_win, 0, 0, 1, 2);
    gtk_grid_attach(GTK_GRID(main_win->grid), main_win->loc_discr_win->scroll_win, 2, 0, 1, 2);

    /*adding grid to main_win->window*/
    gtk_container_add(GTK_CONTAINER(main_win->window), main_win->grid);
}

static void connect_signals()
{
    /*connect for main_win->window*/
    g_signal_connect(main_win->window, "destroy", G_CALLBACK(sp_destroy_main_window), NULL);
    g_signal_connect(main_win->comm_entry, "activate", G_CALLBACK(sp_command_enter), NULL);

    /*connect for init_win->window*/
    g_signal_connect(init_win->window, "destroy", G_CALLBACK(sp_destroy_init_window), NULL);
    g_signal_connect(init_win->button, "clicked", G_CALLBACK(sp_ip_enter), (gpointer)init_win->entry);
    g_signal_connect(init_win->entry, "activate", G_CALLBACK(sp_ip_enter), (gpointer)init_win->entry);
}

void free_mem()
{
    g_free(main_win->main_text_win);
    g_free(main_win->chat_win);
    g_free(main_win->loc_discr_win);

    g_free(main_win);
    g_free(init_win);
}

void gui_init()
{
    main_win = g_malloc(sizeof(main_win_t));
    main_win->main_text_win = g_malloc(sizeof(text_win_t));
    main_win->chat_win = g_malloc(sizeof(text_win_t));
    main_win->loc_discr_win = g_malloc(sizeof(text_win_t));

    init_win = g_malloc(sizeof(init_win_t));

    init_main_window();
    init_init_window();

    connect_signals();
}
