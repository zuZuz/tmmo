#ifndef _MAP_H
#define _MAP_H

#include <gtk/gtk.h>

#define FONT_SET "Arial 15px"

#define MAP_SCALE 9
#define SURFACE_COUNT 7
#define OBJECT_COUNT 2

typedef enum surface_type {
    soil = 0,
    sand,
    grass,
    water,
    wall,
    floor,
    unknown
} surface_type_t;

typedef enum object_type {
    nothing,
    character
} object_type_t;

typedef struct map_point {
    surface_type_t surface;
    object_type_t object;
}map_point_t;

typedef struct map {
    GtkWidget *draw_area;
    cairo_surface_t *surface;

    map_point_t points[MAP_SCALE * MAP_SCALE];
    char pos[16];
}map_t;

gboolean map_refresh(gpointer map);

void map_load(map_point_t* points, map_t* map);

gboolean sp_draw_area_init(GtkWidget *draw_area, GdkEventConfigure *event, gpointer map);
gboolean sp_draw(GtkWidget *widget, cairo_t *cr, gpointer map);

#endif
