#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
const int tX = 16 * 16 * 4;
const int tY = 16 * 16 * 4;

// x,y - pos of top left
// size - sizeo of a tile
typedef struct Camera
{
    float x;
    float y;
    float size;
} Camera;

typedef struct TileType
{
    SDL_Texture *texture;
    unsigned int anim_tile_x;
    unsigned int anim_tile_y;
    unsigned char size_x;
    unsigned char size_y;
    unsigned char x_map;
    unsigned char y_offset;
    unsigned char animation_modulo;
    unsigned char animation_mask;
    unsigned char id;
} TileType;

typedef struct Tile Tile;

typedef struct Tile
{
    Tile *base_tile;
    int x;
    int y;
    unsigned int flags;
    char type;
} Tile;

typedef struct KeyStates
{
    int up : 1;
    int down : 1;
    int left : 1;
    int right : 1;
    int mouse_left : 1;
    int mouse_right : 1;
} KeyStates;

static inline SDL_Rect *rect_in_camera_space(Camera camera, int x, int y, int w, int h)
{
    return &(SDL_Rect){(camera.x + x) * camera.size, (camera.y + y) * camera.size, camera.size * w, camera.size * h};
}

static inline char tile_is_empty(Tile *tile)
{
    return tile->base_tile->type == -1;
}

static inline char tile_is_not_empty(Tile *tile)
{
    return tile->base_tile->type != -1;
}

void render_tile(SDL_Renderer *renderer, Camera camera, Tile *tile, TileType *types, int x, int y, char advance_animation)
{
    if (tile->type != -1)
    {
        TileType type = types[tile->type];
        if (advance_animation && types[tile->type].animation_modulo != 1)
        {
            printf("%d %d %d\n", type.anim_tile_y, ((tile->flags & type.animation_mask) & type.x_map) * type.anim_tile_x, ((tile->flags & type.animation_mask) >> type.y_offset) * type.anim_tile_y);
            tile->flags = (tile->flags & ~(types[tile->type].animation_mask))         // clear animanion_frame
                          | ((tile->flags + 1) % types[tile->type].animation_modulo); // set new animation_frame
        }
        // TODO: animation frame lookups?
        SDL_RenderCopy(renderer, types[tile->type].texture,
                       types[tile->type].animation_modulo == 1 ? NULL : &(SDL_Rect){((tile->flags & type.animation_mask) & type.x_map) * type.anim_tile_x, ((tile->flags & type.animation_mask) >> type.y_offset) * type.anim_tile_y, type.anim_tile_x, type.anim_tile_y},
                       rect_in_camera_space(camera, x, y, types[tile->type].size_x, types[tile->type].size_y));
    }
}

char is_room_for_tile(Tile *tiles, Tile *mouse_tile, TileType type)
{
    for (int x = mouse_tile->x; x < (mouse_tile->x + type.size_x) && x >= 0 && x < tX; x++)
    {
        for (int y = mouse_tile->y; y < (mouse_tile->y + type.size_y) && y >= 0 && y < tY; y++)
        {
            if (tile_is_not_empty(tiles + (y * tY + x)))
            {
                return 0;
            }
        }
    }
    return 1;
}

char tile_place(Tile *tiles, Tile *mouse_tile, TileType type)
{
    if (is_room_for_tile(tiles, mouse_tile, type))
    {
        mouse_tile->type = type.id;
        for (int x = mouse_tile->x; x < (mouse_tile->x + type.size_x) && x >= 0 && x < tX; x++)
        {
            for (int y = mouse_tile->y; y < (mouse_tile->y + type.size_y) && y >= 0 && y < tY; y++)
            {
                tiles[y * tY + x].base_tile = mouse_tile;
            }
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

TileType *tile_destroy(Tile *tiles, Tile *base_tile, TileType *types)
{
    if (tile_is_not_empty(base_tile))
    {
        for (int x = base_tile->x; x < (base_tile->x + types[base_tile->type].size_x) && x >= 0 && x < tX; x++)
        {
            for (int y = base_tile->y; y < (base_tile->y + types[base_tile->type].size_y) && y >= 0 && y < tY; y++)
            {
                tiles[y * tY + x].base_tile = tiles + (y * tY + x);
            }
        }
        base_tile->type = -1;
        return types + base_tile->type;
    }
    else
    {
        return NULL;
    }
}

static inline int get_mouse_id(int x, int y, Camera camera, int type_in_hand, TileType *types)
{
    x = (int)(((float)(x) - (camera.x + ((type_in_hand != -1) ? ((types[type_in_hand].size_x - 1) / 2.0f) : 0)) * camera.size) / camera.size);
    y = (int)(((float)(y) - (camera.y + ((type_in_hand != -1) ? ((types[type_in_hand].size_y - 1) / 2.0f) : 0)) * camera.size) / camera.size);
    return y * tY + x;
}

// For no animation have map_x, map_y, width and height at 0
TileType create_type(SDL_Renderer *renderer, const char *file, int size_x, int size_y, int tile_map_x_pow, int tile_map_y_pow, int text_width, int text_height)
{
    static int texture_type_id;
    SDL_Texture *texture = IMG_LoadTexture(renderer, file);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return (TileType){
        .texture = texture,
        .anim_tile_x = text_width / pow(2.0, tile_map_x_pow),
        .anim_tile_y = text_height / pow(2.0, tile_map_y_pow),
        .size_x = size_x,
        .size_y = size_y,
        .x_map = tile_map_x_pow,
        .y_offset = tile_map_y_pow + 1,
        .animation_modulo = pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow)),
        .animation_mask = ((char)pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow)) - 1),
        .id = texture_type_id++};
}

int main(int argc, char *argv[])
{
    int width = 1920;
    int height = 1080;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Log("lol");

    SDL_Window *window = SDL_CreateWindow(
        "Scuffedtorio",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        // TODO: add VSYNC
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Event event;
    int running = 1;
    // SDL_Texture *chessT = IMG_LoadTexture(renderer, "../images/sprite.png");
    // SDL_Texture *beaconT = IMG_LoadTexture(renderer, "../images/beacon-bottom.png");
    int movement_x = 2;
    int movement_y = 2;
    Camera camera = {1, 1, 100};
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

    // TTF_Font *font = TTF_OpenFont("../data/core/fonts/TitilliumWeb-Regular.ttf", 24);
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
                    width = event.window.data1;
                    height = event.window.data2;
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
        int max_x = fmin(tX, -camera.x + width / camera.size + 1);
        int max_y = fmin(tY, -camera.y + height / camera.size + 1);
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
            // TODO: add src_rect lookup
            SDL_RenderCopy(renderer, types[type_in_hand].texture, NULL, rect_in_camera_space(camera, mouse_tile->x, mouse_tile->y, types[type_in_hand].size_x, types[type_in_hand].size_y));
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
    // TODO: destroy textures

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}