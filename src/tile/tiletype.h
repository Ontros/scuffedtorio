#pragma once
#include "tiletype.c"

SDL_Rect get_animation_rect_general(unsigned int index, TileType type);

TileType create_type(SDL_Renderer *renderer, const char *file, int size_x, int size_y, int tile_map_x_pow, int tile_map_y_pow, int text_width, int text_height);