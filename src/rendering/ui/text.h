#pragma once
#include "../../structs.h"
#include <stdio.h>
#include <SDL2/SDL_ttf.h>
#include "text.c"

Text text_init(char *font_name, int font_size, int buffer_size);

void text_render(SDL_Renderer *renderer, Text text);
void text_create_with_pos(SDL_Renderer *renderer, Text *text, int x, int y);
void text_create(SDL_Renderer *renderer, Text *text);

void text_free(Text text);