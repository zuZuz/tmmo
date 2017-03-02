#include "map.h"
#include <float.h>
#include <time.h>

static double sum_sq(double x, double y)
{
    return x * x + y * y;
}

static ground_type_t nearest_ground(map_point_t *point, map_point_t *sites, size_t sites_cnt)
{
    ground_type_t nearest_ground;
    double dist;
    double dist_min = DBL_MAX;

    for (size_t i = 0; i < sites_cnt; i++)
    {
        dist = sum_sq(point->x - sites[i].x, point->y - sites[i].y);
        if (dist < dist_min)
        {
            dist_min = dist;
            nearest_ground = sites[i].ground;
        }
    }

    return nearest_ground;
}

void set_region(map_point_t *point, map_point_t *map, size_t size_x, size_t size_y, size_t *regions_num)
{
    for(int y = point->y - 1; y <= point->y + 1; y++)
    {
        if(y < 0  ||  y >= size_y)
            continue;

        for(int x = point->x - 1; x <= point->x + x; x++)
        {
            if( x < 0  ||  x >= size_x  ||  (point->x == x && point->y == y) )
                continue;

            if((map + y*size_x + x)->x == x  &&  (map + y*size_x + x)->y == y)
            {
                if((map + y*size_x + x)->ground == point->ground)
                {
                    point->region = (map + y*size_x + x)->region;
                    return;
                }

            }
        }
    }

    point -> region = regions_num[point->ground]++;

}

map_point_t* map_generation(size_t factor, size_t size_x, size_t size_y, ground_type_t bound, size_t indent)
{
    srand(time(NULL));
    rand();

    map_point_t *map = malloc( sizeof(map_point_t) * size_x * size_y );
    size_t sites_cnt = factor * GROUND_TYPE_CNT;
    map_point_t *sites = malloc( (sites_cnt + 8) * sizeof(map_point_t) );
    size_t regions_num[GROUND_TYPE_CNT];

    for(size_t i = 0; i < GROUND_TYPE_CNT; i++)
        regions_num[i] = 0;

    size_t i;

    for(i = 0; i < sites_cnt; i++)
    {
        sites[i].x =  size_x * (double)rand() / RAND_MAX;
        sites[i].y =  size_y * (double)rand() / RAND_MAX;
        sites[i].ground = i % GROUND_TYPE_CNT;
        sites[i].child_object = empty;
    }

    i = 0;
    for(size_t x = 0; x <= size_x; x += size_x/2)
    {
        for(size_t y = 0; y <= size_y; y += size_y/2)
        {
            if(x == size_x/2  &&  y == size_y/2)
                continue;

            sites[i].x = x;
            sites[i].y = y;
            sites[i].ground = bound;
            sites[i].child_object = empty;

            i++;
        }

    }


    for (size_t y = 0; y < size_y; y++)
    {
        for (size_t x = 0; x < size_x; x++)
        {
            (map + y*size_x + x)->x = x;
            (map + y*size_x + x)->y = y;

            (map + y*size_x + x)->ground = y < indent || y > (size_y - indent) || x < indent || x > (size_x - indent) ?
                                           bound:
                                           nearest_ground(map + y*size_x + x, sites, sites_cnt);

            (map + y*size_x + x)->child_object = nothing;
            (map + y*size_x + x)->surface = empty;

            set_region(map + y*size_x + x, map, size_x, size_y, regions_num);
        }
    }

    free(sites);

    return map;
}

