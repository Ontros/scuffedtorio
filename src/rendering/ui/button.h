#pragma once
#include "../../structs.h"
#include "text.h"
#include "../../logic/tile.h"
#include "../../logic/spawner.h"
#include "../../logic/entity.h"
#include "button.c"

Button button_init(SDL_Renderer *renderer, const char *text, int font_size, SDL_Rect rect, void (*on_click)(void *, void *, void *));

void button_render(SDL_Renderer *renderer, Button *button, int mouse_x, int mouse_y);

void button_expand_concrete_click(void *renderer, void *button, void *state);

// 1 - something has been clicked, 0 - nothing has been clicked
char button_container_click(SDL_Renderer *renderer, GameState *state, ButtonContainer container, int mouse_x, int mouse_y);

void button_container_render(SDL_Renderer *renderer, ButtonContainer container, int mouse_x, int mouse_y);

ButtonContainer button_container_in_game_create(SDL_Renderer *renderer);
ButtonContainer button_container_menu_create(SDL_Renderer *renderer);

void button_container_center(ButtonContainer container, int width);

void button_container_free(ButtonContainer container);