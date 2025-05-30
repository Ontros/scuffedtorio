#include "spawner.h"

// 1 - valid, 0 - invalid
char spawner_is_spot_valid(Tile *tiles, int xS, int yS)
{
    for (int x = xS - 2; x < xS + 7; x++)
    {
        for (int y = yS - 2; y < yS + 7; y++)
        {
            if (x < 0 || x >= tX || y < 0 || y >= tY || tiles[y * tY + x].terrain != 0 || tiles[y * tY + x].base_tile)
            {
                return 0;
            }
        }
    }
    return 1;
}

void spawner_place(Tile *tiles, Tile *mouse_tile, TileType type)
{
    BuiltTile *built = (BuiltTile *)(malloc(sizeof(BuiltTile)));
    mouse_tile->type = type.id;
    built->health = type.max_health;
    built->type = type.id;
    built->tile = mouse_tile;
    for (int x = mouse_tile->x; x < (mouse_tile->x + type.size_x) && x >= 0 && x < tX; x++)
    {
        for (int y = mouse_tile->y; y < (mouse_tile->y + type.size_y) && y >= 0 && y < tY; y++)
        {
            tiles[y * tY + x].base_tile = built;
        }
    }
}

SpawnerContainer spawner_spawn(Tile *tiles, GameState state, TileType spawner_type)
{
    SpawnerContainer container = (SpawnerContainer){
        .amount = state.wave.spawner_count,
        .spawner_indecies = (int *)(malloc(sizeof(int) * state.wave.spawner_count))};
    int real_count = 0;
    for (int i = 0; i < container.amount; i++)
    {
        for (int n = 0; n < 100; n++)
        {
            int x = rand() % tX;
            int y = rand() % tY;
            if (spawner_is_spot_valid(tiles, x, y))
            {
                spawner_place(tiles, tiles + (y * tY + x), spawner_type);
                container.spawner_indecies[real_count] = y * tY + x;
                real_count++;
                n = 100;
            }
        }
    }
    container.amount = real_count;
    return container;
}

void spawner_free(SpawnerContainer container)
{
    free(container.spawner_indecies);
}
