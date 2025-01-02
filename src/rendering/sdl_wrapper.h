#pragma once
#include "../structs.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "sdl_wrapper.c"

int sdl_init(Camera camera, SDL_Window **window, SDL_Renderer **renderer);
int sdl_init_mid_game(Camera camera, SDL_Window **window, SDL_Renderer **renderer);