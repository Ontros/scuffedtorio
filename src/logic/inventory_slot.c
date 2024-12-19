#include "inventory_slot.h"
#include "../structs.h"
const int inventory_slots = 5;

InventorySlot inventory_slot_create(SDL_Renderer *renderer, const char *file, const char *name, int starting_count)
{
    static int id = 0;
    SDL_Texture *texture = IMG_LoadTexture(renderer, file);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    InventorySlot slot = {
        .texture = texture,
        .rect = {0, 0, 64, 64},
        .name = SDL_strdup(name),
        .count = starting_count,
        .id = id++};
    return slot;
}

InventorySlot *inventory_init(SDL_Renderer *renderer)
{
    InventorySlot *inventory = (InventorySlot *)(malloc(sizeof(InventorySlot) * inventory_slots));
    inventory[0] = inventory_slot_create(renderer, "../data/base/graphics/icons/iron-plate.png", "Iron", 20);
    inventory[1] = inventory_slot_create(renderer, "../data/base/graphics/icons/copper-plate.png", "Copper", 20);
    inventory[2] = inventory_slot_create(renderer, "../data/base/graphics/icons/coal.png", "Coal", 0);
    inventory[3] = inventory_slot_create(renderer, "../data/base/graphics/icons/stone.png", "Stone", 0);
    inventory[4] = inventory_slot_create(renderer, "../data/base/graphics/icons/fluid/crude-oil.png", "Oil", 0);
    return inventory;
}

void inventory_free(InventorySlot *inventory)
{
    for (int i = 0; i < inventory_slots; i++)
    {
        SDL_DestroyTexture(inventory[i].texture);
        free(inventory[i].name);
    }
    free(inventory);
}
