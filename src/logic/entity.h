#pragma once
#include "../structs.h"
#include "tile.h"
#include "entity.c"

EntityType *entity_types_init(SDL_Renderer *renderer);

void entity_spawn(Entity *entity, Tile *tiles, SpawnerContainer container, char type, EntityType *types, GameState state);

Entity entity_create();

EntityContainer entity_container_create(int amount);

void entity_render(SDL_Renderer *renderer, Camera camera, Entity *entity, EntityType *types, SDL_Texture *fire_texture);

void entity_move(Entity *entity, EntityType *types, Tile *tiles, TileType *tile_types, GameState *state, int *score);

void entity_type_free(EntityType *types);

void entity_container_free(EntityContainer container);