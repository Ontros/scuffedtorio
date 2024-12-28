#include "camera.h"

static inline int get_mouse_id(int x, int y, Camera camera, int type_in_hand, TileType *types)
{
    x = (int)(((float)(x) - (camera.x + ((type_in_hand != -1) ? ((types[type_in_hand].size_x - 1) / 2.0f) : 0)) * camera.size) / camera.size);
    y = (int)(((float)(y) - (camera.y + ((type_in_hand != -1) ? ((types[type_in_hand].size_y - 1) / 2.0f) : 0)) * camera.size) / camera.size);
    return y * tY + x;
}

static inline SDL_Rect *rect_in_camera_space(Camera camera, int x, int y, int w, int h)
{
    return &(SDL_Rect){(camera.x + x) * camera.size, (camera.y + y) * camera.size, camera.size * w, camera.size * h};
}

static inline SDL_Rect *rect_in_camera_space_f(Camera camera, float x, float y, float size, float offset)
{
    return &(SDL_Rect){(camera.x + x - offset) * camera.size, (camera.y + y - offset) * camera.size, camera.size * size, camera.size * size};
}
