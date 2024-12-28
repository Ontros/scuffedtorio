#pragma once
#include "../structs.h"
// #include "../rendering/camera.h"
#include "entity.c"

EntityType *entity_types_init(SDL_Renderer *renderer);

void entity_spawn(Entity *entity, Tile *tiles, SpawnerContainer container, char type, EntityType *types);

Entity entity_create();

EntityContainer entity_container_create(int amount);

void entity_render(SDL_Renderer *renderer, Camera camera, Entity *entity, EntityType *types);