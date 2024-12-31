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

    GameState state = {
        .concrete_radius = 6,
        .concrete_upgrade_cost = 100,
        .inventory = inventory,
        .tiles = tiles,
        .wave_count = 1,
        .wave_current = 0,
        .waves = &(Wave){.enemies_count = 100, .evolution_factor = 10, .spawner_count = 100}};

    // srand(time(NULL));
    uint64_t UPDATE_TIME = SDL_GetPerformanceFrequency() / 60;
    uint64_t NEXT_UPDATE_TIME = SDL_GetPerformanceCounter() + UPDATE_TIME;
    uint64_t SECOND_TIME = SDL_GetPerformanceFrequency();
    uint64_t NEXT_SECOND_TIME = SDL_GetPerformanceCounter() + SECOND_TIME;

    ButtonContainer buttons = button_container_in_game_create(renderer, state);

    SpawnerContainer spawner_container = spawner_spawn(tiles, state, types[5]);
    EntityType *entity_types = entity_types_init(renderer);
    EntityContainer entity_container = entity_container_create(state.waves[state.wave_current].enemies_count);
    for (int i = 0; i < state.waves[state.wave_current].enemies_count; i++)
    {
        entity_spawn(entity_container.entities + i, tiles, spawner_container, 0, entity_types, state);
    }
    BulletList *bullet_list = NULL;
    BulletList *laser_list = NULL;
    FlameList *flame_list = NULL;
    SDL_Texture *flame_texture = flame_texture_get(renderer);

    int frames = 0;
    int updates = 0;
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;
    int animate = 0;

    while (running)
    {
        animate = !(frames % 1000);
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
                if (!updates)
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
                bullet_list_free(laser_list);
                laser_list = NULL;
                bullet_list_free(bullet_list);
                bullet_list = NULL;
                flame_list = flame_list_tick(flame_list, entity_container, entity_types);
                for (int i = 0; i < tX * tY; i++)
                {
                    if (tiles[i].base_tile)
                    {
                        switch (tiles[i].type)
                        {
                            // Gun turret
                        case 2:
                            if ((updates % 4) == 0)
                            {
                                Entity *closest = NULL;
                                float closest_dist = tX * tY * 4; // 4*just to be safe
                                for (int j = 0; j < entity_container.amount; j++)
                                {
                                    if (entity_container.entities[j].is_dead == 0)
                                    {
                                        float dist = pow((float)tiles[i].x - entity_container.entities[j].x, 2) + pow((float)tiles[i].y - entity_container.entities[j].y, 2);
                                        if (dist < pow(types[2].turret_radius, 2) && dist < closest_dist)
                                        {
                                            closest_dist = dist;
                                            closest = entity_container.entities + j;
                                        }
                                    }
                                }
                                if (closest)
                                {
                                    closest->health -= 1.0f;
                                    tiles[i].flags = (unsigned char)(atan2c((float)tiles[i].y - closest->y, closest->x - (float)tiles[i].x) * 128);
                                    // Put new line at start
                                    BulletList *new_list = (BulletList *)(malloc(sizeof(BulletList)));
                                    new_list->next = bullet_list;
                                    bullet_list = new_list;
                                    bullet_list->x_start = (float)tiles[i].x + 1;
                                    bullet_list->y_start = (float)tiles[i].y + 1;
                                    bullet_list->x_end = closest->x;
                                    bullet_list->y_end = closest->y;
                                    if (closest->health <= 0)
                                    {
                                        closest->is_dead = 1;
                                    }
                                }
                            }
                            break;
                        // Laser turret
                        case 3:
                            Entity *closest = NULL;
                            float closest_dist = tX * tY * 4; // 4*just to be safe
                            for (int j = 0; j < entity_container.amount; j++)
                            {
                                if (entity_container.entities[j].is_dead == 0)
                                {
                                    float dist = pow((float)tiles[i].x - entity_container.entities[j].x, 2) + pow((float)tiles[i].y - entity_container.entities[j].y, 2);
                                    if (dist < pow(types[3].turret_radius, 2) && dist < closest_dist)
                                    {
                                        closest_dist = dist;
                                        closest = entity_container.entities + j;
                                    }
                                }
                            }
                            if (closest)
                            {
                                closest->health -= 1.0f;
                                tiles[i].flags = (unsigned char)(atan2c((float)tiles[i].y - closest->y, closest->x - (float)tiles[i].x) * 64);
                                // Put new line at start
                                BulletList *new_list = (BulletList *)(malloc(sizeof(BulletList)));
                                new_list->next = laser_list;
                                laser_list = new_list;
                                laser_list->x_start = (float)tiles[i].x + 1;
                                laser_list->y_start = (float)tiles[i].y + 1;
                                laser_list->x_end = closest->x;
                                laser_list->y_end = closest->y;
                                if (closest->health <= 0)
                                {
                                    closest->is_dead = 1;
                                }
                            }
                            break;
                            // Flame turret
                        case 6:
                            if ((updates % 8) == 0)
                            {
                                Entity *closest = NULL;
                                float closest_dist = tX * tY * 4; // 4*just to be safe
                                for (int j = 0; j < entity_container.amount; j++)
                                {
                                    if (entity_container.entities[j].is_dead == 0)
                                    {
                                        float dist = pow((float)tiles[i].x - entity_container.entities[j].x, 2) + pow((float)tiles[i].y - entity_container.entities[j].y, 2);
                                        if (dist < pow(types[6].turret_radius, 2) && dist < closest_dist)
                                        {
                                            closest_dist = dist;
                                            closest = entity_container.entities + j;
                                        }
                                    }
                                }
                                if (closest)
                                {
                                    closest->health -= 1.0f;
                                    tiles[i].flags = (unsigned char)(atan2c(((float)tiles[i].y - closest->y), -(closest->x - (float)tiles[i].x)) * 64);
                                    // Put new line at start
                                    FlameList *new_list = (FlameList *)(malloc(sizeof(FlameList)));
                                    new_list->next = flame_list;
                                    flame_list = new_list;
                                    flame_list->x = (float)tiles[i].x + 1;
                                    flame_list->y = (float)tiles[i].y + 1;
                                    flame_list->x_end = closest->x;
                                    flame_list->y_end = closest->y;
                                    flame_list->live_left = 180;
                                    if (closest->health <= 0)
                                    {
                                        closest->is_dead = 1;
                                    }
                                }
                            }
                            break;

                        default:
                            break;
                        }
                    }
                }

                if (updates % 2)
                {
                    // Reset occupations
                    for (int i = 0; i < tX * tY; i++)
                    {
                        if (tiles[i].entity_occupied)
                            tiles[i].entity_occupied--;
                    }
                    // Move entities
                    for (int i = 0; i < entity_container.amount; i++)
                    {
                        if (entity_container.entities[i].is_dead == 0)
                        {
                            // entity_container.entities[i].animation++;
                            // entity_container.entities[i].animation %= 16;
                            // Reset moving state
                            entity_move(entity_container.entities + i, entity_types, tiles, types);
                        }
                    }
                }
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
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                // Movement
                if (event.key.keysym.sym == SDLK_w)
                {
                    keyStates.up = 1;
                }
                else if (event.key.keysym.sym == SDLK_s)
                {
                    keyStates.down = 1;
                }
                else if (event.key.keysym.sym == SDLK_a)
                {
                    keyStates.left = 1;
                }
                else if (event.key.keysym.sym == SDLK_d)
                {
                    keyStates.right = 1;
                }
                // Key picking
                else if (event.key.keysym.sym == SDLK_1)
                {
                    type_in_hand = 0;
                }
                else if (event.key.keysym.sym == SDLK_2)
                {
                    type_in_hand = 1;
                }
                else if (event.key.keysym.sym == SDLK_3)
                {
                    type_in_hand = 2;
                }
                else if (event.key.keysym.sym == SDLK_4)
                {
                    type_in_hand = 3;
                }
                else if (event.key.keysym.sym == SDLK_5)
                {
                    type_in_hand = 6;
                }
                else if (event.key.keysym.sym == SDLK_q)
                {
                    animate = 1;
                    if (mouse_tile && mouse_tile->base_tile && type_in_hand == -1)
                    {
                        type_in_hand = mouse_tile->base_tile->type;
                    }
                    else
                    {
                        type_in_hand = -1;
                    }
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                if (event.key.keysym.sym == SDLK_w)
                {
                    keyStates.up = 0;
                }
                else if (event.key.keysym.sym == SDLK_s)
                {
                    keyStates.down = 0;
                }
                else if (event.key.keysym.sym == SDLK_a)
                {
                    keyStates.left = 0;
                }
                else if (event.key.keysym.sym == SDLK_d)
                {
                    keyStates.right = 0;
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
                    if (button_container_click(renderer, &state, buttons, mouse_x, mouse_y))
                    {
                        keyStates.mouse_left = 0;
                        keyStates.mouse_right = 0;
                    }
                    else
                    {
                        keyStates.mouse_left = 1;
                        keyStates.mouse_right = 0;
                    }
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    keyStates.mouse_left = 0;
                    keyStates.mouse_right = 1;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    keyStates.mouse_left = 0;
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    keyStates.mouse_right = 0;
                }
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.type == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    camera->width = event.window.data1;
                    camera->height = event.window.data2;
                }
            }
        }
        // Camera movement
        if (keyStates.up)
        {
            camera->y += deltaTime * camera->speed_factor / camera->size;
        }
        if (keyStates.down)
        {
            camera->y -= deltaTime * camera->speed_factor / camera->size;
        }
        if (keyStates.left)
        {
            camera->x += deltaTime * camera->speed_factor / camera->size;
        }
        if (keyStates.right)
        {
            camera->x -= deltaTime * camera->speed_factor / camera->size;
        }
        if (keyStates.mouse_left)
        {
            // Place
            if (mouse_tile && (type_in_hand != -1) && types[type_in_hand].cost_count && tile_place(tiles, mouse_tile, types[type_in_hand]))
            {
                // Remove resources
                for (int i = 0; i < types[type_in_hand].cost_count; i++)
                {
                    inventory_slot_update(renderer, inventory, types[type_in_hand].costs[i].item, -types[type_in_hand].costs[i].count);
                }
            }
        }
        else if (keyStates.mouse_right)
        {
            // Remove
            Tile tile = tiles[get_mouse_id(mouse_x, mouse_y, *camera, -1, types)];
            if (mouse_tile && tile.base_tile && (tile.base_tile->type != -1) && types[tile.base_tile->type].cost_count)
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
                render_tile(renderer, *camera, tiles + (y * tY + x), types, x, y, animate);
            }
        }

        for (int i = 0; i < entity_container.amount; i++)
        {
            entity_render(renderer, *camera, entity_container.entities + i, entity_types, flame_texture);
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
        button_container_render(renderer, buttons, mouse_x, mouse_y);
        SDL_RenderPresent(renderer);
    }

    tile_free(tiles);
    text_free(fps_text);

    type_free(types, type_amount);
    type_free(ore_types, ore_amount);
    type_free(terrain_types, terrain_amount);

    button_container_free(buttons);
    spawner_free(spawner_container);

    entity_type_free(entity_types);
    entity_container_free(entity_container);

    bullet_list_free(laser_list);
    bullet_list_free(bullet_list);
    flame_list_free(flame_list);

    inventory_free(inventory);
}