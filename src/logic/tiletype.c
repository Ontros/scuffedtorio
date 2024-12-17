#include "tiletype.h"
#include "../structs.h"
// TODO: update
const int type_amount = 6;

SDL_Rect get_animation_rect_general(unsigned int index, TileType type)
{
    return (SDL_Rect){((index & type.animation_mask) & type.x_map) * type.anim_tile_x, ((index & type.animation_mask) >> type.y_offset) * type.anim_tile_y, type.anim_tile_x, type.anim_tile_y};
}

TileType type_create_full(int texture_type_id, SDL_Renderer *renderer, const char *file, int size_x, int size_y)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, file);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    TileType type = {
        .texture = texture,
        .anim_tile_x = 0,
        .anim_tile_y = 0,
        .size_x = size_x,
        .size_y = size_y,
        .x_map = 0,
        .y_offset = 0,
        .animation_modulo = 1,
        .animation_mask = 0,
        .id = texture_type_id,
        .animation_rects = NULL};

    return type;
}

TileType type_create_animation(int texture_type_id, SDL_Renderer *renderer, const char *file, int size_x, int size_y, int tile_map_x_pow, int tile_map_y_pow, int text_width, int text_height)
{
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
        .id = texture_type_id};

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

TileType type_create_static_section(int texture_type_id, SDL_Renderer *renderer, const char *file, int size_x, int size_y, int tile_map_x_pow, int tile_map_y_pow, int text_width, int text_height)
{
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
        .id = texture_type_id};

    type.animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect) * pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow))));
    for (int i = 0; i <= type.animation_modulo; i++)
    {
        type.animation_rects[i] = get_animation_rect_general(i, type);
    }
    type.animation_modulo = 1;

    return type;
}

TileType *types_init(SDL_Renderer *renderer)
{
    TileType *types = malloc(sizeof(TileType) * type_amount);
    types[0] = type_create_full(0, renderer, "../images/beacon-bottom.png", 1, 1);
    types[1] = type_create_full(1, renderer, "../images/beacon-bottom.png", 2, 2);
    types[2] = type_create_animation(2, renderer, "../data/base/graphics/entity/assembling-machine-1/assembling-machine-1.png", 3, 3, 3, 2, 1712, 904);
    types[3] = type_create_full(3, renderer, "../images/beacon-bottom.png", 4, 4);
    types[4] = type_create_full(4, renderer, "../images/sprite.png", 5, 5);
    types[5] = type_create_static_section(5, renderer, "../data/base/graphics/entity/iron-ore/iron-ore.png", 1, 1, 3, 3, 1024, 1024);
    return types;
}

void type_free(TileType type)
{
    SDL_DestroyTexture(type.texture);
    free(type.animation_rects);
}