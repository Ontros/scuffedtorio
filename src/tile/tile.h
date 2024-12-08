#pragma once
#include "../structs.h"
#include <SDL2/SDL.h>
#include "tiletype.h"
#include "tile.c"

static inline char tile_is_empty(Tile *tile);

static inline char tile_is_not_empty(Tile *tile);

void render_tile(SDL_Renderer *renderer, Camera camera, Tile *tile, TileType *types, int x, int y, char advance_animation);

char is_room_for_tile(Tile *tiles, Tile *mouse_tile, TileType type);

char tile_place(Tile *tiles, Tile *mouse_tile, TileType type);

TileType *tile_destroy(Tile *tiles, Tile *base_tile, TileType *types);