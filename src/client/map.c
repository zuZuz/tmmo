#include "map.h"

static GdkPixbuf* surfaces[SURFACE_COUNT];
static GdkPixbuf* objects[OBJECT_COUNT];

static void load_images()
{
    surfaces[soil] = gdk_pixbuf_new_from_file("map_surf/soil.jpg", NULL);
    surfaces[sand] = gdk_pixbuf_new_from_file("map_surf/sand.jpg", NULL);
    surfaces[grass] = gdk_pixbuf_new_from_file("map_surf/grass.jpg", NULL);
    surfaces[water] = gdk_pixbuf_new_from_file("map_surf/water.jpg", NULL);
    surfaces[empty] = gdk_pixbuf_new_from_file("map_surf/empty.jpg", NULL);
    surfaces[wall] = gdk_pixbuf_new_from_file("map_surf/wall.jpg", NULL);
    surfaces[floor] = gdk_pixbuf_new_from_file("map_surf/floor.jpg", NULL);
    surfaces[unknown] = gdk_pixbuf_new_from_file("map_surf/unknown.jpg", NULL);

    objects[character] = gdk_pixbuf_new_from_file("map_obj/character.jpg", NULL);
}

static void clear_surface (cairo_surface_t *surface)
{
    cairo_t *cr;

    cr = cairo_create (surface);

    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);

    cairo_destroy (cr);
}

gboolean sp_draw(GtkWidget *widget, cairo_t *cr, gpointer map)
{
    cairo_set_source_surface(cr, ((map_t*)map)->surface, 0, 0);
    cairo_paint(cr);

    return FALSE;
}

gboolean sp_draw_area_init(GtkWidget *draw_area, GdkEventConfigure *event, gpointer map)
{
    ((map_t*)map)->surface = gdk_window_create_similar_surface(gtk_widget_get_window(GTK_WIDGET(draw_area)),
                                                     CAIRO_CONTENT_COLOR,
                                                     gtk_widget_get_allocated_width(draw_area),
                                                     gtk_widget_get_allocated_height(draw_area));

    clear_surface(((map_t*)map)->surface);

    load_images();

    for(int i = 0; i < MAP_SCALE*MAP_SCALE; i++)
    {
        ((map_t*)map)->points[i].object = nothing;
        ((map_t*)map)->points[i].surface = unknown;
    }

    map_refresh((map_t*)map);

    return TRUE;
}

void map_load(map_point_t* points, map_t* map)
{
    for(int i = 0; i < MAP_SCALE*MAP_SCALE; i++)
    {
        map->points[i].surface = points[i].surface;
        map->points[i].object = points[i].object;
    }
}

gboolean map_refresh(gpointer map)
{
    double x_scale, y_scale;
    GdkPixbuf *pixbuf;

    x_scale = gtk_widget_get_allocated_width(((map_t*)map)->draw_area)/MAP_SCALE;
    y_scale = gtk_widget_get_allocated_height(((map_t*)map)->draw_area)/MAP_SCALE;

    cairo_t *cr;
    cr = cairo_create(((map_t*)map)->surface);

    for(int i = 0; i < MAP_SCALE*MAP_SCALE; i++)
    {
        pixbuf = surfaces[((map_t*)map)->points[i].surface > SURFACE_COUNT ? unknown : ((map_t*)map)->points[i].surface];

        gdk_cairo_set_source_pixbuf(cr, pixbuf, (i/MAP_SCALE)*x_scale, (i%MAP_SCALE)*y_scale);
        cairo_paint(cr);


        if(((map_t*)map)->points[i].object == nothing) continue;
        else pixbuf = objects[((map_t*)map)->points[i].object];

        gdk_cairo_set_source_pixbuf(cr, pixbuf, ((i/MAP_SCALE)*x_scale) + (x_scale - gdk_pixbuf_get_width(pixbuf))/2, ((i%MAP_SCALE)*y_scale) + (x_scale - gdk_pixbuf_get_height(pixbuf))/2);
        cairo_paint(cr);
    }

    cairo_destroy(cr);

    gtk_widget_queue_draw(((map_t*)map)->draw_area);

    return TRUE;
}
