#pragma once
#include "../structs.h"
#include "inventory_slot.c"

InventorySlot *inventory_init(SDL_Renderer *renderer);

void inventory_free(InventorySlot *inventory);