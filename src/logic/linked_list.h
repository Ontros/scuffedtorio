#pragma once
#include "../structs.h"
#include "linked_list.c"

void bullet_list_free(BulletList *list);
void bullet_list_render(SDL_Renderer *renderer, Camera camera, BulletList *laser_list);

void flame_list_free(FlameList *list);
FlameList *flame_list_tick(FlameList *list, EntityContainer container, EntityType *types);
void flame_list_render(SDL_Renderer *renderer, Camera camera, FlameList *flame_list, SDL_Texture *texture);

SDL_Texture *flame_texture_get(SDL_Renderer *renderer);