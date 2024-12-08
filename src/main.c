#include "rendering/camera.h"
#include "rendering/sdl_wrapper.h"
#include "logic/tile.h"

int main(int argc, char *argv[])
{
    Camera camera = {1, 1, 100, 1920, 1080};
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    sdl_init(camera, &window, &renderer);

    SDL_Event event;
    int running = 1;
    int movement_x = 2;
    int movement_y = 2;
    float camera_speed_factor = 7;
    float camera_scroll_factor = 1;
    KeyStates keyStates = {0, 0, 0, 0, 0, 0};
    Tile *tiles = (Tile *)(malloc(sizeof(Tile) * tX * tY));
    // TileType types[] = {{beaconT, 1, 1, 0}, {chessT, 2, 2, 1}, {beaconT, 3, 3, 2}, {chessT, 4, 4, 3}, {beaconT, 5, 5, 4}};
    TileType types[] = {
        create_type(renderer, "../images/beacon-bottom.png", 1, 1, 0, 0, 0, 0),
        create_type(renderer, "../images/beacon-bottom.png", 2, 2, 0, 0, 0, 0),
        create_type(renderer, "../data/base/graphics/entity/assembling-machine-1/assembling-machine-1.png", 3, 3, 3, 2, 1712, 904),
        create_type(renderer, "../images/beacon-bottom.png", 4, 4, 0, 0, 0, 0),
        create_type(renderer, "../images/sprite.png", 5, 5, 0, 0, 0, 0),
    };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int x = 0; x < tX; x++)
    {
        for (int y = 0; y < tY; y++)
        {
            Tile *cur_tile = tiles + (y * tY + x);
            cur_tile->base_tile = cur_tile;
            cur_tile->type = -1;
            cur_tile->flags = 0;
            cur_tile->x = x;
            cur_tile->y = y;
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    }

    int mouse_id, type_in_hand, mouse_x, mouse_y;
    Tile *mouse_tile = NULL;

    TTF_Init();
    TTF_Font *font = TTF_OpenFont("../data/core/fonts/TitilliumWeb-SemiBold.ttf", 24);
    SDL_Color text_color = {255, 255, 255};
    printf("%d\n", font == NULL);

    char fps_buffer[50];
    SDL_Texture *fps_texture;
    SDL_Surface *fps_surface;
    Uint32 last_frame = SDL_GetTicks();
    int frames = 0;
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;
    int animate = 0;
    while (running)
    {
        tile_place(tiles, tiles, types[2]);
        animate = 1;
        // delta time
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

        // Mouse position
        SDL_GetMouseState(&mouse_x, &mouse_y);
        mouse_id = get_mouse_id(mouse_x, mouse_y, camera, type_in_hand, types);
        if (mouse_id >= 0 && mouse_id < tX * tY)
        {
            if (type_in_hand == -1)
            {
                mouse_tile = tiles[mouse_id].base_tile;
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
                    type_in_hand = 4;
                }
                else if (event.key.keysym.sym == SDLK_q)
                {
                    animate = 1;
                    if (mouse_tile && type_in_hand == -1)
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
                camera.size += (float)event.wheel.y * camera_scroll_factor;
                if (camera.size < 1)
                {
                    camera.size = 1;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    keyStates.mouse_left = 1;
                    keyStates.mouse_right = 0;
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
                    camera.width = event.window.data1;
                    camera.height = event.window.data2;
                }
            }
        }
        // Camera movement
        if (keyStates.up)
        {
            camera.y += deltaTime * camera_speed_factor / camera.size;
        }
        if (keyStates.down)
        {
            camera.y -= deltaTime * camera_speed_factor / camera.size;
        }
        if (keyStates.left)
        {
            camera.x += deltaTime * camera_speed_factor / camera.size;
        }
        if (keyStates.right)
        {
            camera.x -= deltaTime * camera_speed_factor / camera.size;
        }
        if (keyStates.mouse_left)
        {
            // Place
            if (mouse_tile && (type_in_hand != -1) && tile_place(tiles, mouse_tile, types[type_in_hand]))
            {
                // Remove resources
            }
        }
        else if (keyStates.mouse_right)
        {
            // Remove
            if (mouse_tile)
            {
                TileType *destroyed_type = tile_destroy(tiles, tiles[get_mouse_id(mouse_x, mouse_y, camera, -1, types)].base_tile, types);
                // Add resources
            }
        }

        // Background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 19, 133, 16, 255);
        SDL_RenderFillRect(renderer, rect_in_camera_space(camera, 0, 0, tX, tY));

        // Tile rendering
        int max_x = fmin(tX, -camera.x + camera.width / camera.size + 1);
        int max_y = fmin(tY, -camera.y + camera.height / camera.size + 1);
        for (int x = fmax(0, -camera.x - 3); x < max_x; x++)
        {
            for (int y = fmax(0, -camera.y - 3); y < max_y; y++)
            {
                render_tile(renderer, camera, tiles + (y * tY + x), types, x, y, animate);
            }
        }

        // Placing preview
        if (type_in_hand != -1 && mouse_tile)
        {
            // Render preview
            SDL_SetTextureAlphaMod(types[type_in_hand].texture, 128);
            SDL_RenderCopy(renderer, types[type_in_hand].texture, types[type_in_hand].animation_rects, rect_in_camera_space(camera, mouse_tile->x, mouse_tile->y, types[type_in_hand].size_x, types[type_in_hand].size_y));
            SDL_SetTextureAlphaMod(types[type_in_hand].texture, 255);
            // Render blocking tiles
            for (int x = mouse_tile->x; x < (mouse_tile->x + types[type_in_hand].size_x) && x >= 0 && x < tX; x++)
            {
                for (int y = mouse_tile->y; y < (mouse_tile->y + types[type_in_hand].size_y) && y >= 0 && y < tY; y++)
                {
                    if (tile_is_not_empty(tiles + (y * tY + x)))
                    {
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 196);
                        SDL_RenderFillRect(renderer, rect_in_camera_space(camera, x, y, 1, 1));
                    }
                }
            }
            // Rectangle around
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderDrawRect(renderer, rect_in_camera_space(camera, mouse_tile->x, mouse_tile->y, types[type_in_hand].size_x, types[type_in_hand].size_y));
        }
        // Hover over highlight
        else if (mouse_tile && mouse_tile->type != -1)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 224, 255);
            SDL_RenderDrawRect(renderer, rect_in_camera_space(camera, mouse_tile->x, mouse_tile->y, types[mouse_tile->type].size_x, types[mouse_tile->type].size_y));
        }

        frames++;
        if (SDL_GetTicks() - last_frame >= 1000)
        {
            SDL_FreeSurface(fps_surface);
            SDL_DestroyTexture(fps_texture);
            sprintf(fps_buffer, "FPS: %d", frames);
            fps_surface = TTF_RenderText_Solid(font, fps_buffer, text_color);
            fps_texture = SDL_CreateTextureFromSurface(renderer, fps_surface);
            frames = 0;
            last_frame += 1000;
        }

        if (fps_texture)
            SDL_RenderCopy(renderer, fps_texture, NULL, &(SDL_Rect){0, 0, fps_surface->w, fps_surface->h});

        SDL_RenderPresent(renderer);
    }

    free(tiles);
    TTF_CloseFont(font);
    // TODO: destroy textures, tiles and types

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}