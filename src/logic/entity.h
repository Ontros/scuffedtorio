#pragma once
#include "../structs.h"
// #include "../rendering/camera.h"
#include "entity.c"

EntityType *entity_types_init(SDL_Renderer *renderer);

Entity entity_spawn(float x, float y, char type, EntityType *types);

void entity_render(SDL_Renderer *renderer, Camera camera, Entity *entity, EntityType *types);