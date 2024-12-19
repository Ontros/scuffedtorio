#include "tile.h"

static inline char tile_is_empty(Tile *tile)
{
    return tile->base_tile->type == -1;
}

static inline char tile_is_not_empty(Tile *tile)
{
    return tile->base_tile->type != -1;
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
        for (int x = base_tile->x; x < (base_tile->x + types[base_tile->type].size_x) && x >= 0 && x < tX; x++)
        {
            for (int y = base_tile->y; y < (base_tile->y + types[base_tile->type].size_y) && y >= 0 && y < tY; y++)
            {
                tiles[y * tY + x].base_tile = tiles + (y * tY + x);
            }
        }
        base_tile->type = -1;
        return types + base_tile->type;
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
            cur_tile->x = x;
            cur_tile->y = y;
        }
    }
    return tiles;
}