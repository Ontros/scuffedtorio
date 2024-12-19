#pragma once
#include <SDL2/SDL.h>
#include "tiletype.c"

TileType *types_init(SDL_Renderer *rederer);

void type_free(TileType type);