#pragma once
#include "../structs.h"
#include "linked_list.c"

void laser_list_free(LaserList *list);
void laser_list_render(SDL_Renderer *renderer, Camera camera, LaserList *laser_list);