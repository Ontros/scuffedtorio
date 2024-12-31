#pragma once
#include "../structs.h"
#include "input.c"

void input_handler(KeyStates *keyStates, int *running, int *type_in_hand, Tile *mouse_tile, Camera *camera, double deltaTime,
                   InventorySlot *inventory, SDL_Renderer *renderer, GameState *state, ButtonContainer buttons, int mouse_x, int mouse_y,
                   Tile *tiles, TileType *types, Text *missing_materials_text, int *missing_materials_duration);