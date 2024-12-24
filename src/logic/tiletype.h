#pragma once
#include <SDL2/SDL.h>
#include "tiletype.c"

TileType *types_init(SDL_Renderer *renderer);

TileType *types_ore_init(SDL_Renderer *renderer);

TileType *types_terrain_init(SDL_Renderer *renderer);

void type_free(TileType type);