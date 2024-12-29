#include "sdl_wrapper.h"

int sdl_init(Camera camera, SDL_Window **window, SDL_Renderer **renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    *window = SDL_CreateWindow(
        "Scuffedtorio",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        camera.width,
        camera.height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (*window == NULL)
    {
        printf("error: %s\n", SDL_GetError());
        return 1;
    }
    *renderer = SDL_CreateRenderer(
        *window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (*renderer == NULL)
    {
        printf("error: %s\n", SDL_GetError());
        return 1;
    }
    if (SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND))
    {
        printf("error: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init())
    {
        printf("error: %s\n", TTF_GetError());
        return 1;
    }
    return 0;
}