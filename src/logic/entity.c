#include "entity.h"

EntityType entity_type_init(EntityTexture *run, EntityTexture *attack, float damage, float hp, float size_x, float size_y)
{
    return (EntityType){
        .texture_running = run,
        .texture_attack = attack,
        .damage = damage,
        .max_health = hp,
        .size_x = size_x,
        .size_y = size_y,
    };
}

SDL_Texture *texture_load(SDL_Renderer *renderer, char *file)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, file);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return texture;
}

EntityTexture *entity_texture_create_run(SDL_Renderer *renderer)
{
    EntityTexture *output = (EntityTexture *)(malloc(sizeof(EntityTexture)));
    output->animation_mask = 15;
    output->animation_modulo = 16;
    output->texture[0] = texture_load(renderer, "../data/base/graphics/entity/biter/biter-run-1.png");
    output->texture[1] = texture_load(renderer, "../data/base/graphics/entity/biter/biter-run-2.png");
    output->texture[2] = texture_load(renderer, "../data/base/graphics/entity/biter/biter-run-3.png");
    output->texture[3] = texture_load(renderer, "../data/base/graphics/entity/biter/biter-run-4.png");
    output->animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect)));
    const int tile_w = 398;
    const int tile_h = 310;
    for (int i = 0; i < 8; i++)
    {
        output->animation_rects[i] = (SDL_Rect){tile_w * i, 0, tile_w, tile_h};
        output->animation_rects[i + 8] = (SDL_Rect){tile_w * i, tile_h, tile_w, tile_h};
    }
    return output;
}

EntityTexture *entity_texture_create_attack(SDL_Renderer *renderer)
{
    EntityTexture *output = (EntityTexture *)(malloc(sizeof(EntityTexture)));
    output->animation_mask = 15;
    output->animation_modulo = 11;
    output->texture[0] = texture_load(renderer, "../data/base/graphics/entity/biter/biter-attack-1.png");
    output->texture[1] = texture_load(renderer, "../data/base/graphics/entity/biter/biter-attack-2.png");
    output->texture[2] = texture_load(renderer, "../data/base/graphics/entity/biter/biter-attack-3.png");
    output->texture[3] = texture_load(renderer, "../data/base/graphics/entity/biter/biter-attack-4.png");
    output->animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect)));
    const int tile_w = 356;
    const int tile_h = 348;
    for (int i = 0; i < 12; i++)
    {
        output->animation_rects[i] = (SDL_Rect){tile_w * i, 0, tile_w, tile_h};
    }
    return output;
}

EntityType *entity_types_init(SDL_Renderer *renderer)
{
    EntityType *types = (EntityType *)(malloc(sizeof(EntityType) * 1));
    EntityTexture *run = entity_texture_create_run(renderer);
    EntityTexture *attack = entity_texture_create_attack(renderer);
    types[0] = entity_type_init(run, attack, 10, 100, 2, 2);
    return types;
}

Entity entity_spawn(float x, float y, char type, EntityType *types)
{
    return (Entity){
        .animation = 0,
        .health = types[type].max_health,
        .target_x = -1,
        .target_y = -1,
        .type = type,
        .x = x,
        .y = y,
        .is_dead = 0};
}

// All types have the same texture so we can directly acces types
void entity_render(SDL_Renderer *renderer, Camera camera, Entity *entity, EntityType *types)
{
    if (entity->is_dead == 0)
    {
        EntityType *type = types + entity->type;
        EntityTexture *entity_texture = (entity->animation & 0b1000000) ? types->texture_attack : types->texture_running;
        SDL_RenderCopy(renderer,
                       entity_texture->texture[(entity->animation >> 4) & 0b11],
                       entity_texture->animation_rects + (entity->animation & entity_texture->animation_mask),
                       rect_in_camera_space_f(camera, entity->x, entity->y, type->size_x, type->size_y));
    }
}
