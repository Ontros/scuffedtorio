#include "sdl_wrapper.h"

int sdl_init(Camera camera, SDL_Window **window, SDL_Renderer **renderer)
{
    SDL_Init(SDL_INIT_VIDEO);
    printf("error: %s\n", SDL_GetError());

    *window = SDL_CreateWindow(
        "Scuffedtorio",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        camera.width,
        camera.height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    printf("error: %s\n", SDL_GetError());
    *renderer = SDL_CreateRenderer(
        *window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    printf("error: %s\n", SDL_GetError());
    SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND);
    TTF_Init();
}