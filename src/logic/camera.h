#pragma once
#include "../structs.h"
#include "camera.c"

static inline int get_mouse_id(int x, int y, Camera camera, int type_in_hand, TileType *types);

static inline SDL_Rect *rect_in_camera_space(Camera camera, int x, int y, int w, int h);