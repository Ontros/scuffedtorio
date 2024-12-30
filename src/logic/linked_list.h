#pragma once
#include "../structs.h"
#include "linked_list.c"

void bullet_list_free(BulletList *list);
void bullet_list_render(SDL_Renderer *renderer, Camera camera, BulletList *laser_list);