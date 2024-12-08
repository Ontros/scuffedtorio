#pragma once
#include <SDL2/SDL.h>
#include "tiletype.c"

TileType *types_init(SDL_Renderer *rederer);

SDL_Rect get_animation_rect_general(unsigned int index, TileType type);

TileType create_type(SDL_Renderer *renderer, const char *file, int size_x, int size_y, int tile_map_x_pow, int tile_map_y_pow, int text_width, int text_height);

void type_free(TileType type);