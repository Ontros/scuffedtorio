#pragma once
#include "../structs.h"
#include "inventory_slot.c"

InventorySlot *inventory_init(SDL_Renderer *renderer);

void inventory_free(InventorySlot *inventory);

void inventory_slot_update(SDL_Renderer *renderer, InventorySlot *inventory, int id, int count_delta);

void inventory_render(SDL_Renderer *renderer, InventorySlot *inventory);