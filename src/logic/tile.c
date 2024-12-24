#include "tile.h"

static inline char tile_is_empty(Tile *tile)
{
    return tile->base_tile->type == -1 && tile->terrain != 1;
}

static inline char tile_is_not_empty(Tile *tile)
{
    return tile->base_tile->type != -1 || tile->terrain == 1;
}

void render_tile(SDL_Renderer *renderer, Camera camera, Tile *tile, TileType *types, int x, int y, char advance_animation)
{
    if (tile->type != -1)
    {
        TileType type = types[tile->type];
        if (advance_animation && types[tile->type].animation_modulo != 1)
        {
            printf("%d %d %d\n", type.anim_tile_y, ((tile->flags & type.animation_mask) & type.x_map) * type.anim_tile_x, ((tile->flags & type.animation_mask) >> type.y_offset) * type.anim_tile_y);
            tile->flags = (tile->flags & ~(types[tile->type].animation_mask))         // clear animanion_frame
                          | ((tile->flags + 1) % types[tile->type].animation_modulo); // set new animation_frame
        }
        SDL_RenderCopy(renderer, types[tile->type].texture,
                       types[tile->type].animation_rects + (tile->flags & types[tile->type].animation_mask),
                       rect_in_camera_space(camera, x, y, types[tile->type].size_x, types[tile->type].size_y));
    }
}

void render_ore(SDL_Renderer *renderer, Camera camera, Tile *tile, TileType *ore_types, int x, int y)
{
    if (tile->ore != -1)
    {
        TileType type = ore_types[tile->ore];
        SDL_RenderCopy(renderer, type.texture,
                       type.animation_rects + (tile->flags & type.animation_mask),
                       rect_in_camera_space(camera, x, y, type.size_x, type.size_y));
    }
}

void render_terrain(SDL_Renderer *renderer, Camera camera, Tile *tile, TileType *terrain_types, int x, int y)
{
    TileType type = terrain_types[tile->terrain];
    SDL_RenderCopy(renderer, type.texture,
                   type.animation_rects + (tile->flags & type.animation_mask),
                   rect_in_camera_space(camera, x, y, type.size_x, type.size_y));
}

char is_room_for_tile(Tile *tiles, Tile *mouse_tile, TileType type)
{
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
        mouse_tile->type = type.id;
        for (int x = mouse_tile->x; x < (mouse_tile->x + type.size_x) && x >= 0 && x < tX; x++)
        {
            for (int y = mouse_tile->y; y < (mouse_tile->y + type.size_y) && y >= 0 && y < tY; y++)
            {
                tiles[y * tY + x].base_tile = mouse_tile;
            }
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

TileType *tile_destroy(Tile *tiles, Tile *base_tile, TileType *types)
{
    if (tile_is_not_empty(base_tile))
    {
        int og_type = base_tile->type;
        for (int x = base_tile->x; x < (base_tile->x + types[base_tile->type].size_x) && x >= 0 && x < tX; x++)
        {
            for (int y = base_tile->y; y < (base_tile->y + types[base_tile->type].size_y) && y >= 0 && y < tY; y++)
            {
                tiles[y * tY + x].base_tile = tiles + (y * tY + x);
            }
        }
        base_tile->type = -1;
        return types + og_type;
    }
    else
    {
        return NULL;
    }
}

Tile *tiles_malloc()
{
    Tile *tiles = (Tile *)(malloc(sizeof(Tile) * tX * tY));
    for (int x = 0; x < tX; x++)
    {
        for (int y = 0; y < tY; y++)
        {
            Tile *cur_tile = tiles + (y * tY + x);
            cur_tile->base_tile = cur_tile;
            cur_tile->type = -1;
            cur_tile->flags = 0;
            cur_tile->health = -1;
            cur_tile->terrain = 0;
            cur_tile->ore = -1;
            cur_tile->x = x;
            cur_tile->y = y;
        }
    }
    return tiles;
}

void tile_add_ore_patch(Tile *tiles, int ore_id, int patch_size, int x, int y)
{
    int cur_x = x;
    int cur_y = y;
    // Clamping to int to make patches higher
    int side_length = (int)sqrt(patch_size);
    for (int i = 0; i < patch_size; i++)
    {
        // Clamp values
        cur_x = fmin(tX, fmax(0, cur_x));
        cur_y = fmin(tY, fmax(0, cur_y));
        tiles[cur_y * tY + cur_x].ore = ore_id;
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
