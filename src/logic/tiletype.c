#include "tiletype.h"
#include "../structs.h"
#include <string.h>
// TODO: update
const int type_amount = 6;
const int ore_amount = 5;
const int terrain_amount = 4;

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

TileType type_create_base(const char *name, unsigned char size_x, unsigned char size_y)
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
        .name = SDL_strdup(name),
        .size_x = size_x,
        .size_y = size_y,
        .texture = NULL,
        .textures_count = 0,
        .x_map = 0,
        .y_offset = 0};
    return type;
}

void type_add_costs(TileType *type, int costs_count, Tile_Cost *costs)
{
    type->costs = (Tile_Cost *)(malloc(sizeof(Tile_Cost) * costs_count));
    memcpy(type->costs, costs, sizeof(Tile_Cost) * costs_count);
    type->cost_count = costs_count;
}

TileType *types_init(SDL_Renderer *renderer)
{
    TileType *types = malloc(sizeof(TileType) * type_amount);
    types[0] = type_create_base("Wall", 1, 1);
    type_add_static_section(types, renderer, "../data/base/graphics/entity/wall/wall-single.png", 1, 0, 128, 86);
    type_add_costs(types, 1, (Tile_Cost[]){{3, 5}});
    types[1] = type_create_base("Electric mining drill", 3, 3);
    type_add_full_texture(types + 1, renderer, "../data/base/graphics/entity/electric-mining-drill/electric-mining-drill-N.png");
    type_add_costs(types + 1, 2, (Tile_Cost[]){{0, 10}, {1, 10}});
    types[2] = type_create_base("Gun Turret", 2, 2);
    type_add_static_section(types + 2, renderer, "../data/base/graphics/entity/gun-turret/gun-turret-shooting-1.png", 1, 4, 264, 2080);
    type_add_costs(types + 2, 2, (Tile_Cost[]){{0, 10}, {1, 10}});
    types[3] = type_create_base("Laser turret", 2, 2);
    type_add_static_section(types + 3, renderer, "../data/base/graphics/entity/laser-turret/laser-turret-shooting.png", 3, 3, 1008, 960);
    type_add_costs(types + 3, 2, (Tile_Cost[]){{0, 10}, {1, 10}});
    types[4] = type_create_base("Rocket silo", 9, 9);
    type_add_full_texture(types + 4, renderer, "../data/base/graphics/entity/rocket-silo/06-rocket-silo.png");
    types[5] = type_create_base("Biter spawner", 5, 5);
    type_add_static_section(types + 5, renderer, "../data/base/graphics/entity/spawner/spawner-idle.png", 2, 3, 2080, 3008);
    return types;
}

TileType *types_ore_init(SDL_Renderer *renderer)
{
    TileType *types = malloc(sizeof(TileType) * ore_amount);
    types[0] = type_create_base("Iron Ore", 1, 1);
    type_add_static_section(types, renderer, "../data/base/graphics/entity/iron-ore/iron-ore.png", 3, 3, 1024, 1024);
    types[1] = type_create_base("Copper Ore", 1, 1);
    type_add_static_section(types + 1, renderer, "../data/base/graphics/entity/copper-ore/copper-ore.png", 3, 3, 1024, 1024);
    types[2] = type_create_base("Coal Ore", 1, 1);
    type_add_static_section(types + 2, renderer, "../data/base/graphics/entity/coal/coal.png", 3, 3, 1024, 1024);
    types[3] = type_create_base("Stone Ore", 1, 1);
    type_add_static_section(types + 3, renderer, "../data/base/graphics/entity/stone/stone.png", 3, 3, 1024, 1024);
    types[4] = type_create_base("Uranium Ore", 1, 1);
    type_add_static_section(types + 4, renderer, "../data/base/graphics/entity/uranium-ore/uranium.png", 3, 3, 1024, 1024);
    return types;
}

TileType *types_terrain_init(SDL_Renderer *renderer)
{
    TileType *types = malloc(sizeof(TileType) * terrain_amount);
    SDL_Rect top_left_rect = {0, 0, 64, 64};
    types[0] = type_create_base("Grass", 1, 1);
    type_add_full_texture(types, renderer, "../data/base/graphics/terrain/grass-1.png");
    types[0].animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect)));
    memcpy(types[0].animation_rects, &top_left_rect, sizeof(SDL_Rect));

    types[1] = type_create_base("Water", 1, 1);
    type_add_full_texture(types + 1, renderer, "../data/base/graphics/terrain/water/water1.png");
    types[1].animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect)));
    memcpy(types[1].animation_rects, &top_left_rect, sizeof(SDL_Rect));

    types[2] = type_create_base("Concrete", 1, 1);
    type_add_full_texture(types + 2, renderer, "../data/base/graphics/terrain/concrete/refined-concrete.png");
    types[2].animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect)));
    memcpy(types[2].animation_rects, &top_left_rect, sizeof(SDL_Rect));

    top_left_rect = (SDL_Rect){0, 0, 86, 86};
    types[3] = type_create_base("Hazard Concrete", 1, 1);
    type_add_full_texture(types + 3, renderer, "../data/base/graphics/terrain/hazard-concrete-left/refined-hazard-concrete-left.png");
    types[3].animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect)));
    memcpy(types[3].animation_rects, &top_left_rect, sizeof(SDL_Rect));

    return types;
}

void type_free(TileType type)
{
    SDL_DestroyTexture(type.texture);
    free(type.animation_rects);
}