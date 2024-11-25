#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
const int tX = 16 * 16;
const int tY = 16 * 16;

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
    char size_x;
    char size_y;
    // TODO: remove, you can get it by subtract
    char id;
} TileType;

typedef struct Tile Tile;

typedef struct Tile
{
    Tile *base_tile;
    int x;
    int y;
    char type;
    char flags;
} Tile;

typedef struct KeyStates
{
    int up : 1;
    int down : 1;
    int left : 1;
    int right : 1;
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

void render_tile(SDL_Renderer *renderer, Camera camera, Tile *tile, TileType *types, int x, int y)
{
    if (tile_is_not_empty(tile))
    {
        SDL_RenderCopy(renderer, types[tile->type].texture, NULL, rect_in_camera_space(camera, x, y, types[tile->type].size_x, types[tile->type].size_y));
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

int main(int argc, char *argv[])
{
    const int width = 1920;
    const int height = 1080;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Scuffedtorio",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Event event;
    int running = 1;
    SDL_Texture *chessT = IMG_LoadTexture(renderer, "../images/sprite.png");
    SDL_Texture *beaconT = IMG_LoadTexture(renderer, "../images/beacon-bottom.png");
    int movement_x = 2;
    int movement_y = 2;
    Camera camera = {0, 0, 100};
    float camera_speed_factor = 21;
    float camera_scroll_factor = 1;
    KeyStates keyStates = {0, 0, 0, 0};
    Tile *tiles = (Tile *)(malloc(sizeof(Tile) * tX * tY));
    TileType types[] = {{chessT, 1, 1, 0}, {beaconT, 3, 3, 1}};
    SDL_SetTextureBlendMode(types[0].texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(types[1].texture, SDL_BLENDMODE_BLEND);
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

    int mouseX, mouseY, mouse_x, mouse_y, mouse_id, type_in_hand;
    Tile *mouse_tile = NULL;
    while (running)
    {
        // Mouse position
        SDL_GetMouseState(&mouseX, &mouseY);
        mouse_x = (int)(((float)mouseX - (camera.x + ((mouse_tile != NULL) ? (types[type_in_hand].size_x / 2) : 0)) * camera.size) / camera.size);
        mouse_y = (int)(((float)mouseY - (camera.y + ((mouse_tile != NULL) ? (types[type_in_hand].size_y / 2) : 0)) * camera.size) / camera.size);
        mouse_id = mouse_y * tY + mouse_x;
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
                else if (event.key.keysym.sym == SDLK_q)
                {
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
                camera.size += event.wheel.y * camera_scroll_factor;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    // Place
                    if (mouse_tile && tile_place(tiles, mouse_tile, types[type_in_hand]))
                    {
                        // Remove resources
                    }
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    if (mouse_tile)
                    {
                        TileType *destroyed_type = tile_destroy(tiles, mouse_tile->base_tile, types);
                        // Add resources
                    }
                }
            }
        }
        // Camera movement
        if (keyStates.up)
        {
            camera.y += camera_speed_factor / camera.size;
        }
        if (keyStates.down)
        {
            camera.y -= camera_speed_factor / camera.size;
        }
        if (keyStates.left)
        {
            camera.x += camera_speed_factor / camera.size;
        }
        if (keyStates.right)
        {
            camera.x -= camera_speed_factor / camera.size;
        }

        // Background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 19, 133, 16, 255);
        SDL_RenderFillRect(renderer, rect_in_camera_space(camera, 0, 0, tX, tY));

        // Tile rendering
        for (int x = 0; x < tX; x++)
        {
            for (int y = 0; y < tY; y++)
            {
                render_tile(renderer, camera, tiles + (y * tY + x), types, x, y);
            }
        }

        // Placing preview
        if (type_in_hand != -1 && mouse_tile)
        {
            // Render preview
            SDL_SetTextureAlphaMod(types[type_in_hand].texture, 128);
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

        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / 60);
    }

    free(tiles);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}