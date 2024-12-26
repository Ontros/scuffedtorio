#pragma once
#include "../../structs.h"
#include "text.h"
#include "button.c"

Button button_init(SDL_Renderer *renderer, const char *text, int font_size, SDL_Rect rect, void *on_click);

void button_change_text(SDL_Renderer *renderer, Button *button, const char *text);

void button_render(SDL_Renderer *renderer, Button *button, int mouse_x, int mouse_y);