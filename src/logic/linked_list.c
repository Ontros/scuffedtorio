#include "linked_list.h"
const float flame_speed = 0.5f;

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

void flame_list_free(FlameList *list)
{
    if (list)
    {
        flame_list_free(list->next);
        free(list);
    }
}

FlameList *flame_list_tick(FlameList *list, EntityContainer container, EntityType *types)
{
    if (!list)
    {
        return NULL;
    }
    list->next = flame_list_tick(list->next, container, types);
    // Is alive?
    list->live_left--;
    if (list->live_left == 0)
    {
        FlameList *output = list->next;
        free(list);
        return output;
    }
    // Move
    float x_dist = list->x_end - list->x;
    float y_dist = list->y_end - list->y;
    float size = sqrt(pow(x_dist, 2) + pow(y_dist, 2));
    // Landed
    if (size <= 1)
    {
        // Set on fire
        for (int i = 0; i < container.amount; i++)
        {
            Entity *entity = container.entities + i;
            if (entity->is_dead == 0)
            {
                EntityType *type = types + entity->type;
                if ((list->x <= (entity->x + type->size)) && ((list->x + 1) >= (entity->x)) && (list->y <= (entity->y + type->size)) && ((list->y + 1) >= (entity->y)))
                {
                    entity->fire_time_left = 180;
                }
            }
        }
    }
    else
    {
        list->x += x_dist * flame_speed / size;
        list->y += y_dist * flame_speed / size;
    }
    return list;
}

void flame_list_render(SDL_Renderer *renderer, Camera camera, FlameList *flame_list, SDL_Texture *texture)
{
    if (flame_list)
    {
        flame_list_render(renderer, camera, flame_list->next, texture);
        SDL_RenderCopy(renderer, texture,
                       &(SDL_Rect){0, 0, 84, 130},
                       rect_in_camera_space_f(camera, flame_list->x, flame_list->y, 1, 0.5f));
    }
}

SDL_Texture *flame_texture_get(SDL_Renderer *renderer)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, "./data/base/graphics/entity/fire-flame/fire-flame-01.png");
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return texture;
}
