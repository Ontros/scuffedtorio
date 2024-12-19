#include "tiletype.h"
#include "../structs.h"
// TODO: update
const int type_amount = 4;

SDL_Rect get_animation_rect_general(unsigned int index, TileType type)
{
    return (SDL_Rect){((index & type.animation_mask) & type.x_map) * type.anim_tile_x, ((index & type.animation_mask) >> type.y_offset) * type.anim_tile_y, type.anim_tile_x, type.anim_tile_y};
}

void type_add_full_texture(TileType *type, SDL_Renderer *renderer, const char *file)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, file);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    type->texture = texture;
}

TileType type_add_static_section(TileType *type, SDL_Renderer *renderer, const char *file, int tile_map_x_pow, int tile_map_y_pow, int text_width, int text_height)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, file);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    type->texture = texture;
    type->anim_tile_x = text_width / pow(2.0, tile_map_x_pow);
    type->anim_tile_y = text_height / pow(2.0, tile_map_y_pow);
    type->x_map = tile_map_x_pow;
    type->y_offset = tile_map_y_pow + 1;
    int temp_animation_modulo = pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow));
    type->animation_mask = ((char)pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow)) - 1);

    type->animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect) * pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow))));
    for (int i = 0; i <= type->animation_modulo; i++)
    {
        type->animation_rects[i] = get_animation_rect_general(i, *type);
    }
}

// TileType type_add_animation(int texture_type_id, SDL_Renderer *renderer, const char *file, int size_x, int size_y, int tile_map_x_pow, int tile_map_y_pow, int text_width, int text_height)
// {
//     SDL_Texture *texture = IMG_LoadTexture(renderer, file);
//     SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
//     TileType type = {
//         .texture = texture,
//         .anim_tile_x = text_width / pow(2.0, tile_map_x_pow),
//         .anim_tile_y = text_height / pow(2.0, tile_map_y_pow),
//         .size_x = size_x,
//         .size_y = size_y,
//         .x_map = tile_map_x_pow,
//         .y_offset = tile_map_y_pow + 1,
//         .animation_modulo = pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow)),
//         .animation_mask = ((char)pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow)) - 1),
//         .id = texture_type_id};

//     if (type.animation_modulo == 1)
//     {
//         type.animation_rects = NULL;
//     }
//     else
//     {
//         type.animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect) * pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow))));
//         for (int i = 0; i <= type.animation_modulo; i++)
//         {
//             type.animation_rects[i] = get_animation_rect_general(i, type);
//         }
//     }

//     return type;
// }

// TileType type_create_mining_drill()
// {
// }

TileType type_create_base(char *name, unsigned char size_x, unsigned char size_y)
{
    static unsigned char id = 0;
    TileType type = {
        .anim_tile_x = 0,
        .anim_tile_y = 0,
        .animation_mask = 0,
        .animation_modulo = 1,
        .animation_rects = 0,
        .cost_count = 0,
        .costs = NULL,
        .id = id++,
        .max_health = -1,
        .name = name,
        .size_x = size_x,
        .size_y = size_y,
        .texture = NULL,
        .textures_count = 0,
        .x_map = 0,
        .y_offset = 0};
    return type;
}

TileType *types_init(SDL_Renderer *renderer)
{
    TileType *types = malloc(sizeof(TileType) * type_amount);
    types[0] = type_create_base("Electric mining drill", 3, 3);
    type_add_full_texture(types, renderer, "../data/base/graphics/entity/electric-mining-drill/electric-mining-drill-N.png");
    types[1] = type_create_base("Wall", 1, 1);
    type_add_static_section(types + 1, renderer, "../data/base/graphics/entity/wall/wall-single.png", 1, 0, 128, 86);
    types[2] = type_create_base("Gun Turret", 2, 2);
    type_add_static_section(types + 2, renderer, "../data/base/graphics/entity/gun-turret/gun-turret-shooting-1.png", 1, 4, 264, 2080);
    types[3] = type_create_base("Laser turret", 2, 2);
    type_add_static_section(types + 3, renderer, "../data/base/graphics/entity/laser-turret/laser-turret-shooting.png", 3, 3, 1008, 960);
    return types;
}

void type_free(TileType type)
{
    SDL_DestroyTexture(type.texture);
    free(type.animation_rects);
}