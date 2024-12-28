#include "entity.h"
const float mS = 0.1f; // move speed

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
    output->animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect) * 16));
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
    output->animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect) * 12));
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
    types[0] = entity_type_init(run, attack, 10, 100, 2.0f, 2.0f);
    return types;
}

void entity_spawn(Entity *entity, Tile *tiles, SpawnerContainer container, char type, EntityType *types, GameState state)
{
    Tile *spawner = tiles + container.spawner_indecies[rand() % container.amount];
    entity->health = types[type].max_health;
    // float x_offset = (float)(rand() % 400) / 100.0f;
    // if (x_offset > 2.0f)
    // {
    //     x_offset += 5.0f;
    // }
    // float y_offset = (float)(rand() % 400) / 100.0f;
    // if (y_offset > 2.0f)
    // {
    //     y_offset += 5.0f;
    // }
    entity->x = spawner->x - 2;
    entity->y = spawner->y - 2;
    entity->type = type;
    entity->is_dead = 0;
    // TODO: Find target
    entity->target_x = cX;
    entity->target_y = cY;
    entity->moving_to_x = entity->x;
    entity->moving_to_y = entity->y;
}

Entity entity_create()
{
    return (Entity){
        .animation = 0,
        .health = 0,
        .target_x = -1,
        .target_y = -1,
        .moving_to_x = -1,
        .moving_to_y = -1,
        .type = 0,
        .x = -1,
        .y = -1,
        .is_dead = 1};
}

EntityContainer entity_container_create(int amount)
{
    EntityContainer container = (EntityContainer){
        .amount = amount,
        .entities = (Entity *)(malloc(sizeof(Entity) * amount)),
        .spawned = 0,
    };
    for (int i = 0; i < amount; i++)
    {
        container.entities[i] = entity_create();
    }
    return container;
}

// All types have the same texture so we can directly acces types
void entity_render(SDL_Renderer *renderer, Camera camera, Entity *entity, EntityType *types)
{
    EntityType *type = types + entity->type;
    // TODO: entity culling
    if (entity->is_dead == 0)
    {
        EntityTexture *entity_texture = (entity->animation & 0b1000000) ? types->texture_attack : types->texture_running;
        SDL_RenderCopy(renderer,
                       entity_texture->texture[(entity->animation >> 4) & 0b11],
                       entity_texture->animation_rects + (entity->animation & entity_texture->animation_mask),
                       rect_in_camera_space_f(camera, entity->x, entity->y, type->size_x, type->size_y));
    }
}

void entity_move(Entity *entity, EntityType *types, Tile *tiles)
{
    if (entity->x == (float)entity->moving_to_x && entity->y == (float)entity->moving_to_y)
    {
        // There is room for me
        if (tiles[entity->moving_to_y * tY + entity->moving_to_x].entity_occupied == 0)
        {
            tiles[entity->moving_to_y * tY + entity->moving_to_x].entity_occupied = 10;
            int dir = 0;
            int x_dif = entity->target_x - (int)entity->x;
            int y_dif = entity->target_y - (int)entity->y;
            // Horizontal
            if (abs(x_dif) > abs(y_dif))
            {
                // Right
                if (x_dif > 0)
                {
                    dir = 1;
                    entity->moving_to_x++;
                }
                // Left
                else
                {
                    dir = 3;
                    entity->moving_to_x--;
                }
            }
            // Vertical
            else
            {
                // Down
                if (y_dif > 0)
                {
                    dir = 2;
                    entity->moving_to_y++;
                }
                // Up
                else
                {
                    dir = 0;
                    entity->moving_to_y--;
                }
            }
            entity->animation = (dir << 4) | (entity->animation & 0b1111);
        }
        else
        {
            // No room for me, skip
            return;
        }
    }
    switch (entity->animation & 0b110000)
    {
    // Up
    case 0b000000:
        entity->y = fmax(entity->moving_to_y, entity->y - mS);
        break;
    // Right
    case 0b010000:
        entity->x = fmin(entity->moving_to_x, entity->x + mS);
        break;
    // Down
    case 0b100000:
        entity->y = fmin(entity->moving_to_y, entity->y + mS);
        break;
    // Left
    case 0b110000:
        entity->x = fmax(entity->moving_to_x, entity->x - mS);
        break;
    }
    char anim = ((entity->animation & 0b1111) + 1) % 16;
    entity->animation &= 0b1110000;
    entity->animation |= anim;
}
