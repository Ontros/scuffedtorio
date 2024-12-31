#pragma once
#include "../structs.h"
#include <SDL2/SDL.h>
#include "../rendering/camera.h"
#include "tiletype.h"
#include "tile.c"

static inline char tile_is_empty(Tile *tile);

static inline char tile_is_not_empty(Tile *tile);

static inline void render_tile(SDL_Renderer *renderer, Camera camera, Tile *tile, TileType *types, int x, int y, char advance_animation);

static inline void render_ore(SDL_Renderer *renderer, Camera camera, Tile *tile, TileType *types, int x, int y);

static inline void render_terrain(SDL_Renderer *renderer, Camera camera, Tile *tile, TileType *terrain_types, int x, int y);

char is_room_for_tile(Tile *tiles, Tile *mouse_tile, TileType type);

char tile_place(Tile *tiles, Tile *mouse_tile, TileType type);

TileType *tile_destroy(Tile *tiles, BuiltTile *base_tile, TileType *types);

Tile *tiles_malloc(TileType *types);

void tile_add_ore(Tile *tiles, int ore_id, int count);

void tile_add_ore_patch(Tile *tiles, int ore_id, int patch_size, int x, int y);

void tile_create_lake(Tile *tiles, double xS, double yS, double d);

void tile_update_concrete(Tile *tiles, int concrete_radius);

float atan2c(float y, float x);

void tile_free(Tile *tiles);