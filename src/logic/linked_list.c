#include "linked_list.h"

void laser_list_free(LaserList *list)
{
    if (list)
    {
        laser_list_free(list->next);
        free(list);
    }
}

void laser_list_render(SDL_Renderer *renderer, Camera camera, LaserList *laser_list)
{
    if (laser_list)
    {
        laser_list_render(renderer, camera, laser_list->next);
        SDL_RenderDrawLineF(renderer, (camera.x + laser_list->x_start) * camera.size,
                            (camera.y + laser_list->y_start) * camera.size,
                            (camera.x + laser_list->x_end) * camera.size,
                            (camera.y + laser_list->y_end) * camera.size);
    }
}
