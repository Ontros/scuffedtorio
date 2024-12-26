#include "spawner.h"

// 1 - valid, 0 - invalid
char spawner_is_spot_valid(Tile *tiles, int xS, int yS)
{
    for (int x = xS - 2; x < xS + 7; x++)
    {
        for (int y = yS - 2; y < yS + 7; y++)
        {
            if (x < 0 || x >= tX || y < 0 || y >= tY || tiles[y * tY + x].terrain != 0 || tiles[y * tY + x].base_tile->type != -1)
            {
                return 0;
            }
        }
    }
    return 1;
}

char spawner_place(Tile *tiles, Tile *mouse_tile, TileType type)
{

    mouse_tile->type = type.id;
    for (int x = mouse_tile->x; x < (mouse_tile->x + type.size_x) && x >= 0 && x < tX; x++)
    {
        for (int y = mouse_tile->y; y < (mouse_tile->y + type.size_y) && y >= 0 && y < tY; y++)
        {
            tiles[y * tY + x].base_tile = mouse_tile;
        }
    }
}

void spawner_spawn(Tile *tiles, GameState state, TileType spawner_type)
{
    for (int i = 0; i < state.waves[state.wave_current].spawner_count; i++)
    {
        for (int n = 0; n < 100; n++)
        {
            int x = rand() % tX;
            int y = rand() % tY;
            if (spawner_is_spot_valid(tiles, x, y))
            {
                spawner_place(tiles, tiles + (y * tY + x), spawner_type);
                break;
            }
        }
    }
}