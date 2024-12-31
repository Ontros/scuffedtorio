#include "game.h"

int main(int argc, char *argv[])
{
    Camera camera = {-cX + 4, -cY + 4, 100, 1920, 1080, 2, 2, 7, 1};
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    if (sdl_init(camera, &window, &renderer))
    {
        return 1;
    }
    srand(69420);

    game(renderer, &camera);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}