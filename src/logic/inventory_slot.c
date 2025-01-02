#include "inventory_slot.h"
#include "../structs.h"
#include "../rendering/ui/text.h"
const int inventory_slots = 6;

InventorySlot inventory_slot_create(SDL_Renderer *renderer, const char *file, const char *name, int starting_count, int id)
{
    static int start_y = 48;
    if (id == 0)
    {
        start_y = 48;
    }
    InventorySlot slot = {
        .texture = NULL,
        .rect = {0, 0, 64, 64},
        .name = SDL_strdup(name),
        .count = starting_count,
        .id = id};
    slot.text = text_init("./data/core/fonts/TitilliumWeb-SemiBold.ttf", 24, 50);
    sprintf(slot.text.buffer, "%s: %d", slot.name, slot.count);
    text_create_with_pos(renderer, &slot.text, 0, start_y);
    start_y += 24;
    return slot;
}

InventorySlot *inventory_init(SDL_Renderer *renderer)
{
    InventorySlot *inventory = (InventorySlot *)(malloc(sizeof(InventorySlot) * inventory_slots));
    inventory[0] = inventory_slot_create(renderer, "./data/base/graphics/icons/iron-plate.png", "Iron", 243, 0);
    inventory[1] = inventory_slot_create(renderer, "./data/base/graphics/icons/copper-plate.png", "Copper", 243, 1);
    inventory[2] = inventory_slot_create(renderer, "./data/base/graphics/icons/coal.png", "Coal", 0, 2);
    inventory[3] = inventory_slot_create(renderer, "./data/base/graphics/icons/stone.png", "Stone", 0, 3);
    inventory[4] = inventory_slot_create(renderer, "./data/base/graphics/icons/uranium-235.png", "Uranium", 0, 4);
    inventory[5] = inventory_slot_create(renderer, "./data/base/graphics/icons/fluid/crude-oil.png", "Coins", 0, 5);
    return inventory;
}

void inventory_free(InventorySlot *inventory)
{
    for (int i = 0; i < inventory_slots; i++)
    {
        // SDL_DestroyTexture(inventory[i].texture);
        text_free(inventory[i].text);
        free(inventory[i].name);
    }
    free(inventory);
}

void inventory_slot_update(SDL_Renderer *renderer, InventorySlot *inventory, int id, int count_delta)
{
    inventory[id].count += count_delta;
    sprintf(inventory[id].text.buffer, "%s: %d", inventory[id].name, inventory[id].count);
    text_create(renderer, &inventory[id].text);
}

void inventory_render(SDL_Renderer *renderer, InventorySlot *inventory)
{
    for (int i = 0; i < inventory_slots; i++)
    {
        text_render(renderer, inventory[i].text);
    }
}
