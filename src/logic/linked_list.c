#include "linked_list.h"

void bullet_list_free(BulletList *list)
{
    if (list)
    {
        bullet_list_free(list->next);
        free(list);
    }
}

void bullet_list_render(SDL_Renderer *renderer, Camera camera, BulletList *laser_list)
{
    if (laser_list)
    {
        bullet_list_render(renderer, camera, laser_list->next);
        SDL_RenderDrawLineF(renderer, (camera.x + laser_list->x_start) * camera.size,
                            (camera.y + laser_list->y_start) * camera.size,
                            (camera.x + laser_list->x_end) * camera.size,
                            (camera.y + laser_list->y_end) * camera.size);
    }
}
