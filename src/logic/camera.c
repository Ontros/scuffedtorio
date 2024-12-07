#include "camera.h"

static inline int get_mouse_id(int x, int y, Camera camera, int type_in_hand, TileType *types)
{
    x = (int)(((float)(x) - (camera.x + ((type_in_hand != -1) ? ((types[type_in_hand].size_x - 1) / 2.0f) : 0)) * camera.size) / camera.size);
    y = (int)(((float)(y) - (camera.y + ((type_in_hand != -1) ? ((types[type_in_hand].size_y - 1) / 2.0f) : 0)) * camera.size) / camera.size);
    return y * tY + x;
}