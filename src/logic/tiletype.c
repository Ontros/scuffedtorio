#include "tiletype.h"
#include "../structs.h"
#include <string.h>
const int type_amount = 7;
const int ore_amount = 5;
const int terrain_amount = 4;

SDL_Rect get_animation_rect_general(unsigned int index, TileType type)
{
    return (SDL_Rect){((index & type.animation_mask) % type.x_count) * type.anim_tile_x, ((index & type.animation_mask) / type.x_count) * type.anim_tile_y, type.anim_tile_x, type.anim_tile_y};
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
    type->x_count = pow(2.0, tile_map_x_pow);
    int temp_animation_modulo = pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow));
    type->animation_mask = ((char)pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow)) - 1);

    type->animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect) * pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow))));
    for (int i = 0; i <= type->animation_modulo; i++)
    {
        type->animation_rects[i] = get_animation_rect_general(i, *type);
    }
}

TileType type_add_animation(TileType *type, SDL_Renderer *renderer, const char *file, int tile_map_x_pow, int tile_map_y_pow, int text_width, int text_height)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, file);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    type->texture = texture;
    type->anim_tile_x = text_width / pow(2.0, tile_map_x_pow);
    type->anim_tile_y = text_height / pow(2.0, tile_map_y_pow);
    type->x_count = pow(2.0, tile_map_x_pow);
    type->animation_modulo = pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow));
    type->animation_mask = ((char)pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow)) - 1);

    if (type->animation_modulo == 1)
    {
        type->animation_rects = NULL;
    }
    else
    {
        type->animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect) * ((int)type->animation_modulo + 1)));
        for (int i = 0; i <= type->animation_modulo; i++)
        {
            type->animation_rects[i] = get_animation_rect_general(i, *type);
        }
    }
}

TileType type_add_gun_animation(TileType *type, SDL_Renderer *renderer, int tile_map_x_pow, int tile_map_y_pow, int text_width, int text_height)
{
    type->gun_texture[0] = IMG_LoadTexture(renderer, "./data/base/graphics/entity/gun-turret/gun-turret-shooting-1.png");
    SDL_SetTextureBlendMode(type->gun_texture[0], SDL_BLENDMODE_BLEND);
    type->gun_texture[1] = IMG_LoadTexture(renderer, "./data/base/graphics/entity/gun-turret/gun-turret-shooting-2.png");
    SDL_SetTextureBlendMode(type->gun_texture[1], SDL_BLENDMODE_BLEND);
    type->gun_texture[2] = IMG_LoadTexture(renderer, "./data/base/graphics/entity/gun-turret/gun-turret-shooting-3.png");
    SDL_SetTextureBlendMode(type->gun_texture[2], SDL_BLENDMODE_BLEND);
    type->gun_texture[3] = IMG_LoadTexture(renderer, "./data/base/graphics/entity/gun-turret/gun-turret-shooting-4.png");
    SDL_SetTextureBlendMode(type->gun_texture[3], SDL_BLENDMODE_BLEND);
    type->anim_tile_x = text_width / pow(2.0, tile_map_x_pow);
    type->anim_tile_y = text_height / pow(2.0, tile_map_y_pow);
    type->x_count = pow(2.0, tile_map_x_pow);
    type->animation_modulo = pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow));
    type->animation_mask = ((char)pow(2.0, (double)(tile_map_x_pow + tile_map_y_pow)) - 1);

    if (type->animation_modulo == 1)
    {
        type->animation_rects = NULL;
    }
    else
    {
        type->animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect) * ((int)type->animation_modulo + 1)));
        for (int i = 0; i <= type->animation_modulo; i += 2)
        {
            type->animation_rects[i] = get_animation_rect_general(i, *type);
            type->animation_rects[i + 1] = get_animation_rect_general(i, *type);
        }
    }
}

TileType type_create_base(const char *name, unsigned char size_x, unsigned char size_y)
{
    static unsigned char id = 0;
    TileType type = {
        .anim_tile_x = 0,
        .anim_tile_y = 0,
        .gun_texture = NULL,
        .animation_mask = 0,
        .animation_modulo = 1,
        .animation_rects = 0,
        .cost_count = 0,
        .costs = NULL,
        .id = id++,
        .max_health = 100,
        .name = SDL_strdup(name),
        .size_x = size_x,
        .size_y = size_y,
        .texture = NULL,
        .textures_count = 0,
        .x_count = 0,
        .turret_radius = 0,
    };
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
    type_add_static_section(types, renderer, "./data/base/graphics/entity/wall/wall-single.png", 1, 0, 128, 86);
    type_add_costs(types, 1, (Tile_Cost[]){{3, 5}});
    types[1] = type_create_base("Electric mining drill", 3, 3);
    type_add_full_texture(types + 1, renderer, "./data/base/graphics/entity/electric-mining-drill/electric-mining-drill-N.png");
    type_add_costs(types + 1, 2, (Tile_Cost[]){{0, 10}, {1, 10}});
    types[2] = type_create_base("Gun Turret", 2, 2);
    type_add_gun_animation(types + 2, renderer, 1, 4, 264, 2080);
    type_add_costs(types + 2, 2, (Tile_Cost[]){{0, 10}, {1, 10}});
    types[2].animation_modulo = 1;
    types[2].turret_radius = 24.0f;
    types[3] = type_create_base("Laser turret", 2, 2);
    type_add_animation(types + 3, renderer, "./data/base/graphics/entity/laser-turret/laser-turret-shooting.png", 3, 3, 1008, 960);
    type_add_costs(types + 3, 2, (Tile_Cost[]){{0, 10}, {1, 10}});
    types[3].animation_modulo = 1;
    types[3].turret_radius = 16.0f;
    types[4] = type_create_base("Rocket silo", 9, 9);
    type_add_full_texture(types + 4, renderer, "./data/base/graphics/entity/rocket-silo/06-rocket-silo.png");
    types[5] = type_create_base("Biter spawner", 5, 5);
    type_add_static_section(types + 5, renderer, "./data/base/graphics/entity/spawner/spawner-idle.png", 2, 3, 2080, 3008);
    types[6] = type_create_base("Flamethrower", 2, 2);
    type_add_animation(types + 6, renderer, "data/base/graphics/entity/flamethrower-turret/flamethrower-turret-gun.png", 3, 3, 1264, 1024);
    type_add_costs(types + 6, 2, (Tile_Cost[]){{0, 10}, {1, 10}});
    types[6].animation_modulo = 1;
    types[6].turret_radius = 24.0f;
    return types;
}

TileType *types_ore_init(SDL_Renderer *renderer)
{
    TileType *types = malloc(sizeof(TileType) * ore_amount);
    types[0] = type_create_base("Iron Ore", 1, 1);
    type_add_static_section(types, renderer, "./data/base/graphics/entity/iron-ore/iron-ore.png", 3, 3, 1024, 1024);
    types[1] = type_create_base("Copper Ore", 1, 1);
    type_add_static_section(types + 1, renderer, "./data/base/graphics/entity/copper-ore/copper-ore.png", 3, 3, 1024, 1024);
    types[2] = type_create_base("Coal Ore", 1, 1);
    type_add_static_section(types + 2, renderer, "./data/base/graphics/entity/coal/coal.png", 3, 3, 1024, 1024);
    types[3] = type_create_base("Stone Ore", 1, 1);
    type_add_static_section(types + 3, renderer, "./data/base/graphics/entity/stone/stone.png", 3, 3, 1024, 1024);
    types[4] = type_create_base("Uranium Ore", 1, 1);
    type_add_static_section(types + 4, renderer, "./data/base/graphics/entity/uranium-ore/uranium.png", 3, 3, 1024, 1024);
    return types;
}

TileType *types_terrain_init(SDL_Renderer *renderer)
{
    TileType *types = malloc(sizeof(TileType) * terrain_amount);
    SDL_Rect top_left_rect = {0, 0, 64, 64};
    types[0] = type_create_base("Grass", 1, 1);
    type_add_full_texture(types, renderer, "./data/base/graphics/terrain/grass-1.png");
    types[0].animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect)));
    memcpy(types[0].animation_rects, &top_left_rect, sizeof(SDL_Rect));

    types[1] = type_create_base("Water", 1, 1);
    type_add_full_texture(types + 1, renderer, "./data/base/graphics/terrain/water/water1.png");
    types[1].animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect)));
    memcpy(types[1].animation_rects, &top_left_rect, sizeof(SDL_Rect));

    types[2] = type_create_base("Concrete", 1, 1);
    type_add_full_texture(types + 2, renderer, "./data/base/graphics/terrain/concrete/refined-concrete.png");
    types[2].animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect)));
    memcpy(types[2].animation_rects, &top_left_rect, sizeof(SDL_Rect));

    top_left_rect = (SDL_Rect){0, 0, 86, 86};
    types[3] = type_create_base("Hazard Concrete", 1, 1);
    type_add_full_texture(types + 3, renderer, "./data/base/graphics/terrain/hazard-concrete-left/refined-hazard-concrete-left.png");
    types[3].animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect)));
    memcpy(types[3].animation_rects, &top_left_rect, sizeof(SDL_Rect));

    return types;
}

void type_free(TileType *types, int amount)
{
    for (int i = 0; i < amount; i++)
    {
        if (types[i].texture)
        {
            SDL_DestroyTexture(types[i].texture);
            types[i].texture = NULL;
        }
        else
        {
            SDL_DestroyTexture(types[i].gun_texture[0]);
            SDL_DestroyTexture(types[i].gun_texture[1]);
            SDL_DestroyTexture(types[i].gun_texture[2]);
            SDL_DestroyTexture(types[i].gun_texture[3]);
        }
        if (types[i].animation_rects)
            free(types[i].animation_rects);
        free(types[i].name);
        if (types[i].costs)
            free(types[i].costs);
    }
    free(types);
}