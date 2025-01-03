#include "tile.h"
const float pi = 3.14159265359f;

static inline char tile_is_empty(Tile *tile)
{
    return tile->base_tile->type == -1 && tile->terrain > 1;
}

static inline char tile_is_not_empty(Tile *tile)
{
    return tile->base_tile || tile->terrain != 2;
}

static inline void render_tile(SDL_Renderer *renderer, Camera camera, Tile *tile, TileType *types, int x, int y)
{
    if (tile->type != -1)
    {
        TileType *type = types + tile->type;
        if (type->texture)
        {
            SDL_RenderCopy(renderer, types[tile->type].texture,
                           types[tile->type].animation_rects + (tile->flags & types[tile->type].animation_mask),
                           rect_in_camera_space(camera, x, y, types[tile->type].size_x, types[tile->type].size_y));
        }
        else
        {
            SDL_RenderCopy(renderer, types[tile->type].gun_texture[(int)tile->flags / 32],
                           types[tile->type].animation_rects + (tile->flags % 32),
                           rect_in_camera_space(camera, x, y, types[tile->type].size_x, types[tile->type].size_y));
        }
        if (tile->base_tile->health < type->max_health)
        {
            // health background
            SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255);
            SDL_RenderFillRect(renderer, rect_sub_in_camera_space(camera, x, y + (float)type->size_y - 0.2f, (float)type->size_y, 0.2f));
            // health status
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, rect_sub_in_camera_space(camera, x, y + (float)type->size_y - 0.2f, (float)type->size_y * ((float)tile->base_tile->health / (float)type->max_health), 0.2f));
        }
    }
}

static inline void render_ore(SDL_Renderer *renderer, Camera camera, Tile *tile, TileType *ore_types, int x, int y)
{
    if (tile->ore != -1)
    {
        TileType type = ore_types[tile->ore];
        SDL_RenderCopy(renderer, type.texture,
                       type.animation_rects,
                       rect_in_camera_space(camera, x, y, 1, 1));
    }
}

static inline void render_terrain(SDL_Renderer *renderer, Camera camera, Tile *tile, TileType *terrain_types, int x, int y)
{
    if (camera.size >= 10 || tile->terrain != 0)
    {
        TileType type = terrain_types[tile->terrain];
        SDL_RenderCopy(renderer, type.texture,
                       type.animation_rects,
                       rect_in_camera_space(camera, x, y, 1, 1));
    }
}

char is_room_for_tile(Tile *tiles, Tile *mouse_tile, TileType type)
{
    // OOB check
    if (mouse_tile->x < 0 || mouse_tile->x + type.size_x >= tX || mouse_tile->y < 0 || mouse_tile->y + type.size_y >= tY)
    {
        return 0;
    }
    for (int x = mouse_tile->x; x < (mouse_tile->x + type.size_x) && x >= 0 && x < tX; x++)
    {
        for (int y = mouse_tile->y; y < (mouse_tile->y + type.size_y) && y >= 0 && y < tY; y++)
        {
            if (tile_is_not_empty(tiles + (y * tY + x)))
            {
                return 0;
            }
        }
    }
    return 1;
}

char tile_place(Tile *tiles, Tile *mouse_tile, TileType type)
{
    if (is_room_for_tile(tiles, mouse_tile, type))
    {
        BuiltTile *built = (BuiltTile *)(malloc(sizeof(BuiltTile)));
        mouse_tile->type = type.id;
        built->health = type.max_health;
        built->tile = mouse_tile;
        built->type = type.id;
        for (int x = mouse_tile->x; x < (mouse_tile->x + type.size_x) && x >= 0 && x < tX; x++)
        {
            for (int y = mouse_tile->y; y < (mouse_tile->y + type.size_y) && y >= 0 && y < tY; y++)
            {
                tiles[y * tY + x].base_tile = built;
            }
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

TileType *tile_destroy(Tile *tiles, BuiltTile *base_tile, TileType *types)
{
    if (base_tile)
    {
        int og_type = base_tile->type;
        Tile *tile = base_tile->tile;
        for (int x = tile->x; x < (tile->x + types[tile->type].size_x) && x >= 0 && x < tX; x++)
        {
            for (int y = tile->y; y < (tile->y + types[tile->type].size_y) && y >= 0 && y < tY; y++)
            {
                tiles[y * tY + x].base_tile = NULL;
            }
        }
        base_tile->type = -1;
        base_tile->tile->type = -1;
        free(base_tile);
        return types + og_type;
    }
    else
    {
        return NULL;
    }
}

void tile_add_ore_patch(Tile *tiles, int ore_id, int patch_size, int x, int y)
{
    int cur_x = x;
    int cur_y = y;
    // Clamping to int to make patches higher
    int side_length = (int)sqrt(patch_size);
    for (int i = 0; i < patch_size; i++)
    {
        int index = fmin(tY - 1, cur_y) * tY + fmin(tX - 1, cur_x);
        if (tiles[index].terrain != 1)
        {
            tiles[index].ore = ore_id;
        }
        cur_x++;
        if (cur_x - x == side_length)
        {
            cur_x = x;
            cur_y++;
        }
    }
}

void tile_create_lake(Tile *tiles, double xS, double yS, double d)
{
    double r = (double)d / 2.0;
    double r_sqrd = pow(r, 2.0);
    for (int x = xS; x < fmin(xS + d, tX); x++)
    {
        for (int y = yS; y < fmin(yS + d, tY); y++)
        {
            if (pow(xS + r - (double)x, 2.0) + pow((double)yS + r - (double)y, 2.0) < r_sqrd)
            {
                tiles[y * tY + x].terrain = 1;
            }
        }
    }
}

void tile_update_concrete(Tile *tiles, int concrete_radius)
{
    for (int x = cX - concrete_radius - 16; x < cX + concrete_radius + 16; x++)
    {
        for (int y = cY - concrete_radius - 16; y < cY + concrete_radius + 16; y++)
        {
            if (tiles[y * tY + x].terrain != 1)
            {
                if (x < cX - concrete_radius || x > cX + concrete_radius || y < cY - concrete_radius || y > cY + concrete_radius)
                {
                    tiles[y * tY + x].terrain = 3;
                }
                else
                {
                    tiles[y * tY + x].terrain = 2;
                }
            }
        }
    }
}

void tile_add_ore(Tile *tiles, int ore_id, int count)
{
    for (int i = 0; i < count; i++)
    {
        int x = rand() % tX;
        int y = rand() % tY;

        // Prevent ores from spawning in spawn area
        if (x > cX - 8 && x < cX + 8 && y > cY - 8 && y < cY + 8)
        {
            x += 16;
            y += 16;
        }

        tile_add_ore_patch(tiles, ore_id, 9, x, y);
    }
}

Tile *tiles_malloc(TileType *types)
{
    Tile *tiles = (Tile *)(malloc(sizeof(Tile) * tX * tY));
    for (int x = 0; x < tX; x++)
    {
        for (int y = 0; y < tY; y++)
        {
            Tile *cur_tile = tiles + (y * tY + x);
            cur_tile->base_tile = NULL;
            cur_tile->type = -1;
            cur_tile->flags = 0;
            cur_tile->terrain = 0;
            cur_tile->ore = -1;
            cur_tile->x = x;
            cur_tile->y = y;
            cur_tile->entity_occupied = 0;
        }
    }
    tile_create_lake(tiles, 150, 85, 32);
    tile_create_lake(tiles, 300, 450, 16);
    tile_create_lake(tiles, 50, 311, 48);
    tile_create_lake(tiles, 345, 100, 22);
    tile_create_lake(tiles, 200, 200, 32);
    tile_update_concrete(tiles, 16);
    tile_add_ore(tiles, 4, 50);
    tile_add_ore(tiles, 3, 50);
    tile_add_ore(tiles, 2, 50);
    tile_add_ore(tiles, 1, 50);
    tile_add_ore(tiles, 0, 50);
    tile_place(tiles, tiles + ((cY - 4) * tY + cX - 4), types[4]);
    tile_add_ore_patch(tiles, 0, 9, cX + 5, cY - 4);
    tile_add_ore_patch(tiles, 1, 9, cX + 5, cY - 1);
    tile_add_ore_patch(tiles, 3, 9, cX + 5, cY + 2);
    return tiles;
}
// atan2 but top
float atan2c(float y, float x)
{
    float f = atan2(y, x) / pi;
    return (-f + ((f > 0.5f) ? 2.5f : 0.5f)) / 2;
}

void tile_free(Tile *tiles)
{
    for (int i = 0; i < tX * tY; i++)
    {
        if (tiles[i].type != -1)
        {
            free(tiles[i].base_tile);
            tiles[i].base_tile = NULL;
        }
    }
    free(tiles);
}
