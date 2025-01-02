#include "input.h"

char can_build(GameState *game_state)
{
    return (!game_state->is_wave_running) || game_state->can_build_mid_wave;
}

void input_handler(KeyStates *keyStates, int *running, int *type_in_hand, Tile *mouse_tile, Camera *camera, double deltaTime,
                   InventorySlot *inventory, SDL_Renderer *renderer, GameState *state, ButtonContainer buttons, int mouse_x, int mouse_y,
                   Tile *tiles, TileType *types, Text *missing_materials_text, int *missing_materials_duration)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            printf("Exit game\n");
            *running = 0;
            return;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            // Movement
            if (event.key.keysym.sym == SDLK_w)
            {
                keyStates->up = 1;
            }
            else if (event.key.keysym.sym == SDLK_s)
            {
                keyStates->down = 1;
            }
            else if (event.key.keysym.sym == SDLK_a)
            {
                keyStates->left = 1;
            }
            else if (event.key.keysym.sym == SDLK_d)
            {
                keyStates->right = 1;
            }
            // Key picking
            else if (event.key.keysym.sym == SDLK_1)
            {
                *type_in_hand = 0;
            }
            else if (event.key.keysym.sym == SDLK_2)
            {
                *type_in_hand = 1;
            }
            else if (event.key.keysym.sym == SDLK_3)
            {
                *type_in_hand = 2;
            }
            else if (event.key.keysym.sym == SDLK_4)
            {
                *type_in_hand = 3;
            }
            else if (event.key.keysym.sym == SDLK_5)
            {
                *type_in_hand = 6;
            }
            else if (event.key.keysym.sym == SDLK_q)
            {
                if (mouse_tile && mouse_tile->base_tile && *type_in_hand == -1)
                {
                    *type_in_hand = mouse_tile->base_tile->type;
                }
                else
                {
                    *type_in_hand = -1;
                }
            }
        }
        else if (event.type == SDL_KEYUP)
        {
            if (event.key.keysym.sym == SDLK_w)
            {
                keyStates->up = 0;
            }
            else if (event.key.keysym.sym == SDLK_s)
            {
                keyStates->down = 0;
            }
            else if (event.key.keysym.sym == SDLK_a)
            {
                keyStates->left = 0;
            }
            else if (event.key.keysym.sym == SDLK_d)
            {
                keyStates->right = 0;
            }
        }
        else if (event.type == SDL_MOUSEWHEEL)
        {
            camera->size += (float)event.wheel.y * (camera->scroll_factor);
            if (camera->size < 1)
            {
                camera->size = 1;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                if ((state->is_wave_running == 0) && button_container_click(renderer, state, buttons, mouse_x, mouse_y))
                {
                    keyStates->mouse_left = 0;
                    keyStates->mouse_right = 0;
                }
                else
                {
                    keyStates->mouse_left = 1;
                    keyStates->mouse_right = 0;
                }
            }
            else if (event.button.button == SDL_BUTTON_RIGHT)
            {
                keyStates->mouse_left = 0;
                keyStates->mouse_right = 1;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONUP)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                keyStates->mouse_left = 0;
            }
            else if (event.button.button == SDL_BUTTON_RIGHT)
            {
                keyStates->mouse_right = 0;
            }
        }
        else if (event.type == SDL_WINDOWEVENT)
        {
            if (event.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                printf("Exit game\n");
                *running = 0;
                return;
            }
            else if (event.window.type == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                camera->width = event.window.data1;
                camera->height = event.window.data2;
            }
        }
    }
    // Camera movement
    if (keyStates->up)
    {
        camera->y += deltaTime * camera->speed_factor / camera->size;
    }
    if (keyStates->down)
    {
        camera->y -= deltaTime * camera->speed_factor / camera->size;
    }
    if (keyStates->left)
    {
        camera->x += deltaTime * camera->speed_factor / camera->size;
    }
    if (keyStates->right)
    {
        camera->x -= deltaTime * camera->speed_factor / camera->size;
    }
    if (keyStates->mouse_left)
    {
        // Place
        if (can_build(state) && mouse_tile && (*type_in_hand != -1) && types[*type_in_hand].cost_count)
        {
            TileType *t = types + (*type_in_hand);
            char can_be_placed = 1;
            for (int i = 0; i < t->cost_count; i++)
            {
                if (inventory[t->costs[i].item].count < (t->costs[i].count))
                {
                    if (can_be_placed)
                        sprintf(missing_materials_text->buffer, "Missing %dx %s", t->costs[i].count - inventory[t->costs[i].item].count, inventory[t->costs[i].item].name);
                    else
                        sprintf(missing_materials_text->buffer, "%s, %dx %s", missing_materials_text->buffer, t->costs[i].count - inventory[t->costs[i].item].count, inventory[t->costs[i].item].name);
                    can_be_placed = 0;
                }
            }
            if (can_be_placed)
            {
                // Remove resources
                if (tile_place(tiles, mouse_tile, types[*type_in_hand]))
                {
                    for (int i = 0; i < t->cost_count; i++)
                    {
                        inventory_slot_update(renderer, inventory, t->costs[i].item, -t->costs[i].count);
                    }
                }
            }
            else
            {
                text_create_with_pos(renderer, missing_materials_text, mouse_x, mouse_y);
                *missing_materials_duration = 180;
            }
        }
    }
    else if (keyStates->mouse_right)
    {
        // Remove
        Tile tile = tiles[get_mouse_id(mouse_x, mouse_y, *camera, -1, types)];
        if (can_build(state) && mouse_tile && tile.base_tile && (tile.base_tile->type != -1) && types[tile.base_tile->type].cost_count)
        {
            TileType *destroyed_type = tile_destroy(tiles, tile.base_tile, types);
            // Add resources
            if (destroyed_type != NULL)
            {
                for (int i = 0; i < destroyed_type->cost_count; i++)
                {
                    inventory_slot_update(renderer, inventory, destroyed_type->costs[i].item, destroyed_type->costs[i].count);
                }
            }
        }
    }
}