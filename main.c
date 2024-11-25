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
} TileType;

typedef struct Tile Tile;

typedef struct Tile
{
    Tile *base_tile;
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
void render_tile(SDL_Renderer *renderer, Camera camera, Tile *tile, TileType *types, int x, int y)
{
    if (tile == tile->base_tile)
    {
        SDL_Rect rect = {(camera.x + x) * camera.size, (camera.y + y) * camera.size, camera.size * types[tile->type].size_x, camera.size * types[tile->type].size_y};
        SDL_RenderCopy(renderer, types[tile->type].texture, NULL, &rect);
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
    float camera_speed_factor = 0.05;
    float camera_scroll_factor = 1;
    KeyStates keyStates = {0, 0, 0, 0};
    Tile *tiles = (Tile *)(malloc(sizeof(Tile) * tX * tY));
    TileType types[] = {{chessT, 2, 2}, {beaconT, 2, 2}};
    for (int x = 0; x < tX; x++)
    {
        for (int y = 0; y < tY; y++)
        {
            Tile *cur_tile = &tiles[y * tY + x];
            if (x % 2 || y % 2)
            {
                cur_tile->base_tile = &tiles[(y - y % 2) * tY + (x - x % 2)];
            }
            else
            {
                cur_tile->base_tile = cur_tile;
                if ((y + (y % 2) + x) % 4)
                {
                    cur_tile->type = 0;
                }
                else
                {
                    cur_tile->type = 1;
                }
            }
            cur_tile->flags = 0;
        }
    }

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_w)
                {
                    keyStates.up = 1;
                }
                if (event.key.keysym.sym == SDLK_s)
                {
                    keyStates.down = 1;
                }
                if (event.key.keysym.sym == SDLK_a)
                {
                    keyStates.left = 1;
                }
                if (event.key.keysym.sym == SDLK_d)
                {
                    keyStates.right = 1;
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                if (event.key.keysym.sym == SDLK_w)
                {
                    keyStates.up = 0;
                }
                if (event.key.keysym.sym == SDLK_s)
                {
                    keyStates.down = 0;
                }
                if (event.key.keysym.sym == SDLK_a)
                {
                    keyStates.left = 0;
                }
                if (event.key.keysym.sym == SDLK_d)
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
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                // i*s = m - c*s
                int x = (int)(((float)mouseX - camera.x * camera.size) / camera.size);
                int y = (int)(((float)mouseY - camera.y * camera.size) / camera.size);
                int id = y * tY + x;
                if (id >= 0 && id < tX * tY)
                {
                    tiles[id].base_tile->type++;
                    tiles[id].base_tile->type %= 2;
                }
            }
        }
        if (keyStates.up)
        {
            camera.y += camera.size * camera_speed_factor;
        }
        if (keyStates.down)
        {
            camera.y -= camera.size * camera_speed_factor;
        }
        if (keyStates.left)
        {
            camera.x += camera.size * camera_speed_factor;
        }
        if (keyStates.right)
        {
            camera.x -= camera.size * camera_speed_factor;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        for (int x = 0; x < tX; x += 2)
        {
            for (int y = 0; y < tY; y += 2)
            {
                render_tile(renderer, camera, tiles + (y * tY + x), types, x, y);
            }
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