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

typedef struct Tile
{
    int x;
    int y;
    int size_x;
    int size_y;
    SDL_Texture *texture;
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
void render_tile(SDL_Renderer *renderer, Camera camera, Tile tile)
{
    SDL_Rect rect = {(camera.x + tile.x) * camera.size, (camera.y + tile.y) * camera.size, camera.size * tile.size_x, camera.size * tile.size_y};
    SDL_RenderCopy(renderer, tile.texture, NULL, &rect);
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
    for (int x = 0; x < tX; x += 2)
    {
        for (int y = 0; y < tY; y += 2)
        {
            tiles[y * tY + x].x = x;
            tiles[y * tY + x].y = y;
            tiles[y * tY + x].size_x = 2;
            tiles[y * tY + x].size_y = 2;
            if ((y + (y % 2) + x) % 4)
            {
                tiles[y * tY + x].texture = chessT;
            }
            else
            {
                tiles[y * tY + x].texture = beaconT;
            }
            tiles[y * tY + x].flags = 0;
            tiles[y * tY + x].type = 0;
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
                render_tile(renderer, camera, tiles[y * tY + x]);
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