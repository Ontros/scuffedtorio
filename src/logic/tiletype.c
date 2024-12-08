#include "tiletype.h"
#include "../structs.h"
// TODO: update
const int type_amount = 5;

SDL_Rect get_animation_rect_general(unsigned int index, TileType type)
{
    return (SDL_Rect){((index & type.animation_mask) & type.x_map) * type.anim_tile_x, ((index & type.animation_mask) >> type.y_offset) * type.anim_tile_y, type.anim_tile_x, type.anim_tile_y};
}

// For no animation have map_x, map_y, width and height at 0
TileType create_type(SDL_Renderer *renderer, const char *file, int size_x, int size_y, int tile_map_x_pow, int tile_map_y_pow, int text_width, int text_height)
{
    static int texture_type_id;
    SDL_Texture *texture = IMG_LoadTexture(renderer, file);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    TileType type = {
        .texture = texture,
        .anim_tile_x = text_width / pow(2.0, tile_map_x_pow),
        .anim_tile_y = text_height / pow(2.0, tile_map_y_pow),
        .size_x = size_x,
        .size_y = size_y,
        .x_map = tile_map_x_pow,
        .y_offset = tile_map_y_pow + 1,
        .animation_modulo = pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow)),
        .animation_mask = ((char)pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow)) - 1),
        .id = texture_type_id++};

    if (type.animation_modulo == 1)
    {
        type.animation_rects = NULL;
    }
    else
    {
        type.animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect) * pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow))));
        for (int i = 0; i <= type.animation_modulo; i++)
        {
            type.animation_rects[i] = get_animation_rect_general(i, type);
        }
    }

    return type;
}

TileType *types_init(SDL_Renderer *renderer)
{
    TileType *types = malloc(sizeof(TileType) * 5);
    types[0] = create_type(renderer, "../images/beacon-bottom.png", 1, 1, 0, 0, 0, 0);
    types[1] = create_type(renderer, "../images/beacon-bottom.png", 2, 2, 0, 0, 0, 0);
    types[2] = create_type(renderer, "../data/base/graphics/entity/assembling-machine-1/assembling-machine-1.png", 3, 3, 3, 2, 1712, 904);
    types[3] = create_type(renderer, "../images/beacon-bottom.png", 4, 4, 0, 0, 0, 0);
    types[4] = create_type(renderer, "../images/sprite.png", 5, 5, 0, 0, 0, 0);
    return types;
}

void type_free(TileType type)
{
    SDL_DestroyTexture(type.texture);
    free(type.animation_rects);
}