#pragma once
#include "./structs.h"
#include "rendering/camera.h"
#include "rendering/sdl_wrapper.h"
#include "rendering/ui/button.h"
#include "logic/tile.h"
#include "rendering/ui/text.h"
#include "logic/inventory_slot.h"
#include "logic/spawner.h"
#include "logic/entity.h"
#include "logic/linked_list.h"
#include "logic/input.h"
#include "game.c"

int game(SDL_Renderer *renderer, Camera *camera);