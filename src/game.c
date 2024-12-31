#include "game.h"

int game(SDL_Renderer *renderer, Camera *camera)
{
    int score = 0;
    TileType *types = types_init(renderer);
    Tile *tiles = tiles_malloc(types);
    TileType *ore_types = types_ore_init(renderer);
    TileType *terrain_types = types_terrain_init(renderer);
    Tile *rocket_tile = tiles + ((cY - 4) * tY + cX - 4);

    int running = 1;
    KeyStates keyStates = {0, 0, 0, 0, 0, 0};

    int mouse_id = -1;
    int type_in_hand = -1;
    int mouse_x = 0;
    int mouse_y = 0;
    Tile *mouse_tile = NULL;

    Text fps_text = text_init("./data/core/fonts/TitilliumWeb-SemiBold.ttf", 24, 50);
    InventorySlot *inventory = inventory_init(renderer);

    Text missing_materials_text = text_init("./data/core/fonts/TitilliumWeb-SemiBold.ttf", 24, 100);
    int missing_materials_duration = 0;

    GameState state = {
        .concrete_radius = 6,
        .concrete_upgrade_cost = 100,
        .inventory = inventory,
        .tiles = tiles,
        .types = types,
        .is_infinite = 0,
        .can_build_mid_wave = 0,
        .wave_current = 0,
        .entity_types = entity_types_init(renderer),
        .is_wave_running = 0,
        .wave = {.enemies_count = 0, .evolution_factor = 0, .spawner_count = 0}};

    // srand(time(NULL));
    uint64_t UPDATE_TIME = SDL_GetPerformanceFrequency() / 60;
    uint64_t NEXT_UPDATE_TIME = SDL_GetPerformanceCounter() + UPDATE_TIME;
    uint64_t SECOND_TIME = SDL_GetPerformanceFrequency();
    uint64_t NEXT_SECOND_TIME = SDL_GetPerformanceCounter() + SECOND_TIME;

    ButtonContainer buttons = button_container_in_game_create(renderer, state);

    state.spawner_container = spawner_spawn(tiles, state, types[5]);
    state.entity_container = entity_container_create(state.wave.enemies_count);
    BulletList *bullet_list = NULL;
    BulletList *laser_list = NULL;
    FlameList *flame_list = NULL;
    SDL_Texture *flame_texture = flame_texture_get(renderer);

    int frames = 0;
    int updates = 0;
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;

    while (running)
    {
        // delta time
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

        // Every update
        frames++;
        if (NOW > NEXT_UPDATE_TIME)
        {
            if (updates < 60)
            {
                // Add mined resources
                if (!updates && state.is_wave_running)
                {
                    for (int i = 0; i < tX * tY; i++)
                    {
                        if (tiles[i].base_tile && tiles[i].base_tile->type == 1)
                        {
                            if (tiles[i].ore != -1)
                            {
                                inventory[tiles[i].ore].count++;
                            }
                        }
                    }
                    for (int i = 0; i < inventory_slots; i++)
                    {
                        inventory_slot_update(renderer, inventory, i, 0);
                    }
                }

                // Turret attack
                if (state.is_wave_running)
                    turret_tick(tiles, types, updates, &bullet_list, &laser_list, &flame_list, state.entity_container, state.entity_types, &state);

                if (updates % 2 && state.is_wave_running)
                {
                    // Reset occupations
                    for (int i = 0; i < tX * tY; i++)
                    {
                        if (tiles[i].entity_occupied)
                            tiles[i].entity_occupied--;
                    }
                    // Move entities
                    for (int i = 0; i < state.entity_container.amount; i++)
                    {
                        if (state.entity_container.entities[i].is_dead == 0)
                        {
                            // entity_container.entities[i].animation++;
                            // entity_container.entities[i].animation %= 16;
                            // Reset moving state
                            entity_move(state.entity_container.entities + i, state.entity_types, tiles, types, &state);
                        }
                    }
                }

                if (missing_materials_duration)
                {
                    missing_materials_duration--;
                }
            }

            if (state.is_wave_running && !state.entity_container.alive)
            {
                state.is_wave_running = 0;
            }

            updates++;
            NEXT_UPDATE_TIME += UPDATE_TIME;

            // FPS update
            if (NOW > NEXT_SECOND_TIME)
            {
                sprintf(fps_text.buffer, "FPS/UPS: %d/%d", frames, updates);
                text_create_with_pos(renderer, &fps_text, 0, 0);
                frames = 0;
                NEXT_SECOND_TIME += SECOND_TIME;
            }
            if (updates == 60)
            {
                updates = 0;
            }
        }

        // Did game end?
        if (rocket_tile->type == -1)
        {
            running = 0;
            break;
        }

        // Mouse position
        SDL_GetMouseState(&mouse_x, &mouse_y);
        mouse_id = get_mouse_id(mouse_x, mouse_y, *camera, type_in_hand, types);
        // Mouse is in bounds
        if (mouse_id >= 0 && mouse_id < tX * tY)
        {
            if (type_in_hand == -1 && tiles[mouse_id].base_tile)
            {
                mouse_tile = tiles[mouse_id].base_tile->tile;
            }
            else
            {
                mouse_tile = tiles + mouse_id;
            }
        }
        else
        {
            mouse_tile = NULL;
        }

        // Events
        input_handler(&keyStates, &running, &type_in_hand, mouse_tile, camera, deltaTime, inventory, renderer, &state, buttons, mouse_x, mouse_y, tiles, types, &missing_materials_text, &missing_materials_duration);

        // Background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        int max_x = fmin(tX, -camera->x + camera->width / camera->size + 1);
        int max_y = fmin(tY, -camera->y + camera->height / camera->size + 1);
        // Tile+Ore rendering
        if (camera->size < 10)
        {
            SDL_SetRenderDrawColor(renderer, 61, 52, 12, 255);
            SDL_RenderFillRect(renderer, rect_in_camera_space(*camera, 0, 0, tX, tY));
        }
        for (int x = fmax(0, -camera->x - 1); x < max_x; x++)
        {
            for (int y = fmax(0, -camera->y - 1); y < max_y; y++)
            {
                render_terrain(renderer, *camera, tiles + (y * tY + x), terrain_types, x, y);
                render_ore(renderer, *camera, tiles + (y * tY + x), ore_types, x, y);
            }
        }

        // Laser rendering
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        bullet_list_render(renderer, *camera, laser_list);
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        bullet_list_render(renderer, *camera, bullet_list);
        flame_list_render(renderer, *camera, flame_list, flame_texture);

        // Tile rendering
        for (int x = fmax(0, -camera->x - 9); x < max_x; x++)
        {
            for (int y = fmax(0, -camera->y - 9); y < max_y; y++)
            {
                render_tile(renderer, *camera, tiles + (y * tY + x), types, x, y);
            }
        }

        for (int i = 0; i < state.entity_container.amount; i++)
        {
            entity_render(renderer, *camera, state.entity_container.entities + i, state.entity_types, flame_texture);
        }

        // Placing preview
        if (type_in_hand != -1 && mouse_tile)
        {
            // Render preview
            if (types[type_in_hand].texture)
            {
                SDL_SetTextureAlphaMod(types[type_in_hand].texture, 128);
                SDL_RenderCopy(renderer, types[type_in_hand].texture, types[type_in_hand].animation_rects, rect_in_camera_space(*camera, mouse_tile->x, mouse_tile->y, types[type_in_hand].size_x, types[type_in_hand].size_y));
                SDL_SetTextureAlphaMod(types[type_in_hand].texture, 255);
            }
            else
            {
                SDL_SetTextureAlphaMod(types[type_in_hand].gun_texture[0], 128);
                SDL_RenderCopy(renderer, types[type_in_hand].gun_texture[0], types[type_in_hand].animation_rects, rect_in_camera_space(*camera, mouse_tile->x, mouse_tile->y, types[type_in_hand].size_x, types[type_in_hand].size_y));
                SDL_SetTextureAlphaMod(types[type_in_hand].gun_texture[0], 255);
            }
            // Render blocking tiles
            for (int x = mouse_tile->x; x < (mouse_tile->x + types[type_in_hand].size_x) && x >= 0 && x < tX; x++)
            {
                for (int y = mouse_tile->y; y < (mouse_tile->y + types[type_in_hand].size_y) && y >= 0 && y < tY; y++)
                {
                    if (tile_is_not_empty(tiles + (y * tY + x)))
                    {
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 196);
                        SDL_RenderFillRect(renderer, rect_in_camera_space(*camera, x, y, 1, 1));
                    }
                }
            }
            // Rectangle around
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderDrawRect(renderer, rect_in_camera_space(*camera, mouse_tile->x, mouse_tile->y, types[type_in_hand].size_x, types[type_in_hand].size_y));
        }
        // Hover over highlight
        else if (mouse_tile && mouse_tile->base_tile && (mouse_tile->type != -1))
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 224, 255);
            SDL_RenderDrawRect(renderer, rect_in_camera_space(*camera, mouse_tile->x, mouse_tile->y, types[mouse_tile->type].size_x, types[mouse_tile->type].size_y));
        }

        text_render(renderer, fps_text);
        inventory_render(renderer, inventory);
        if (state.is_wave_running == 0)
            button_container_render(renderer, buttons, mouse_x, mouse_y);
        if (missing_materials_duration)
        {
            text_render(renderer, missing_materials_text);
        }
        SDL_RenderPresent(renderer);
    }

    tile_free(tiles);
    text_free(fps_text);

    type_free(types, type_amount);
    type_free(ore_types, ore_amount);
    type_free(terrain_types, terrain_amount);

    button_container_free(buttons);

    spawner_free(state.spawner_container);
    entity_type_free(state.entity_types);
    entity_container_free(state.entity_container);

    bullet_list_free(laser_list);
    bullet_list_free(bullet_list);
    flame_list_free(flame_list);

    inventory_free(inventory);
}