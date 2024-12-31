#include "entity.h"
const int entity_type_count = 1;
const float ticks_to_cross_tile = 2.0f;
const float distance_per_tick = 1.0 / 5.0f; // move speed
const float fire_damage = 1.0f;

static inline char tile_is_pathfindable(Tile *tile)
{
    return tile->terrain != 1 && (!tile->base_tile || tile->base_tile->type != 5);
}

static inline char tile_left_pathfindable(Tile *tiles, int x, int y)
{
    return tile_is_pathfindable(tiles + (y * tY + x - 1));
}
static inline char tile_right_pathfindable(Tile *tiles, int x, int y)
{
    return tile_is_pathfindable(tiles + (y * tY + x + 1));
}
static inline char tile_up_pathfindable(Tile *tiles, int x, int y)
{
    return tile_is_pathfindable(tiles + ((y - 1) * tY + x));
}
static inline char tile_down_pathfindable(Tile *tiles, int x, int y)
{
    return tile_is_pathfindable(tiles + ((y + 1) * tY + x));
}

static inline char tile_is_attackable(Tile *tile)
{
    return !(!tile->base_tile || tile->base_tile->type == 5);
}

char tile_found_path(int move_x, int move_y, int check_x, int check_y, Tile *tiles, int x, int y)
{
    for (int i = 1; i < 10; i++)
    {
        // Found wall
        if (!tile_is_pathfindable(tiles + ((y + i * move_y) * tY + x + i * move_x)))
        {
            return 0;
        }
        // Can go down
        if (tile_is_pathfindable(tiles + ((y + i * move_y + check_y) * tY + x + i * move_x + check_x)))
        {
            return i;
        }
    }
    return 1;
}

EntityType entity_type_init(EntityTexture *run, EntityTexture *attack, float damage, float hp, float size, float offset)
{
    return (EntityType){
        .texture_running = run,
        .texture_attack = attack,
        .damage = damage,
        .max_health = hp,
        .size = size,
        .offset = offset,
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
    output->texture[0] = texture_load(renderer, "./data/base/graphics/entity/biter/biter-run-1.png");
    output->texture[1] = texture_load(renderer, "./data/base/graphics/entity/biter/biter-run-2.png");
    output->texture[2] = texture_load(renderer, "./data/base/graphics/entity/biter/biter-run-3.png");
    output->texture[3] = texture_load(renderer, "./data/base/graphics/entity/biter/biter-run-4.png");
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
    output->texture[0] = texture_load(renderer, "./data/base/graphics/entity/biter/biter-attack-1.png");
    output->texture[1] = texture_load(renderer, "./data/base/graphics/entity/biter/biter-attack-2.png");
    output->texture[2] = texture_load(renderer, "./data/base/graphics/entity/biter/biter-attack-3.png");
    output->texture[3] = texture_load(renderer, "./data/base/graphics/entity/biter/biter-attack-4.png");
    output->animation_rects = (SDL_Rect *)(malloc(sizeof(SDL_Rect) * 12));
    const int tile_w = 356;
    const int tile_h = 348;
    for (int i = 0; i < 12; i++)
    {
        output->animation_rects[i] = (SDL_Rect){tile_w * i, 0, tile_w, tile_h};
    }
    return output;
}

// Offsets:
//  1: 0
//  2: 0.5
//  3: 1
EntityType *entity_types_init(SDL_Renderer *renderer)
{
    EntityType *types = (EntityType *)(malloc(sizeof(EntityType) * entity_type_count));
    EntityTexture *run = entity_texture_create_run(renderer);
    EntityTexture *attack = entity_texture_create_attack(renderer);
    types[0] = entity_type_init(run, attack, 10, 100, 3.0f, 1.0f);
    return types;
}

void entity_set_target(Entity *entity, int target_x, int target_y)
{
    entity->target_x = target_x;
    entity->target_y = target_y;
    int x_dif = entity->target_x - (int)entity->x;
    int y_dif = entity->target_y - (int)entity->y;
    // Horizontal
    if (abs(x_dif) > abs(y_dif))
    {
        if (x_dif > 0)
        {
            entity->main_dir = 1;
        }
        else
        {
            entity->main_dir = 3;
        }
    }
    // Vertical
    else
    {
        if (y_dif > 0)
        {
            entity->main_dir = 2;
        }
        else
        {
            entity->main_dir = 0;
        }
    }
}

void entity_spawn(Entity *entity, Tile *tiles, SpawnerContainer container, char type, EntityType *types, GameState state)
{
    Tile *spawner = tiles + container.spawner_indecies[rand() % container.amount];
    entity->health = types[type].max_health;
    entity->x = spawner->x - 1;
    entity->y = spawner->y - 1;
    entity->type = type;
    entity->is_dead = 0;
    entity->moving_to_x = entity->x;
    entity->moving_to_y = entity->y;
    entity_set_target(entity, cX, cY);
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
        .is_dead = 1,
        .main_dir = 0,
        .fire_time_left = 0};
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
void entity_render(SDL_Renderer *renderer, Camera camera, Entity *entity, EntityType *types, SDL_Texture *fire_texture)
{
    EntityType *type = types + entity->type;
    // TODO: entity culling
    if (entity->is_dead == 0)
    {
        EntityTexture *entity_texture = (entity->animation & 0b1000000) ? types->texture_attack : types->texture_running;
        if (entity->fire_time_left)
        {
            SDL_RenderCopy(renderer,
                           fire_texture,
                           &(SDL_Rect){0, 0, 84, 130},
                           rect_in_camera_space_f(camera, entity->x, entity->y, type->size, type->offset));
        }
        SDL_RenderCopy(renderer,
                       entity_texture->texture[(entity->animation >> 4) & 0b11],
                       entity_texture->animation_rects + (entity->animation & entity_texture->animation_mask),
                       rect_in_camera_space_f(camera, entity->x, entity->y, type->size, type->offset));
        if (entity->health < type->max_health)
        {
            // health background
            SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255);
            SDL_RenderFillRect(renderer, rect_sub_in_camera_space_f(camera, entity->x, entity->y + type->size - 1.0f, type->size, 0.2f, type->offset));
            // helth status
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, rect_sub_in_camera_space_f(camera, entity->x, entity->y + type->size - 1.0f, type->size * (entity->health / type->max_health), 0.2f, type->offset));
        }
    }
}

void entity_move(Entity *entity, EntityType *types, Tile *tiles, TileType *tile_types)
{
    if (entity->fire_time_left)
    {
        entity->fire_time_left--;
        entity->health -= fire_damage;
        if (entity->health <= 0)
        {
            entity->is_dead = 1;
            return;
        }
    }
    if (entity->x == (float)entity->moving_to_x && entity->y == (float)entity->moving_to_y)
    {
        // There is room for me
        if (tiles[entity->moving_to_y * tY + entity->moving_to_x].entity_occupied == 0)
        {
            tiles[entity->moving_to_y * tY + entity->moving_to_x].entity_occupied = (int)ticks_to_cross_tile;
            int dir = 0;
            // - -> Left, + -> Right
            int x_dif = (entity->target_x - (int)entity->x);
            // - -> Up, + -> Down
            int y_dif = (entity->target_y - (int)entity->y);
            char lP = tile_left_pathfindable(tiles, entity->moving_to_x, entity->moving_to_y);
            char luP = tile_left_pathfindable(tiles, entity->moving_to_x, entity->moving_to_y - 1);
            char ldP = tile_left_pathfindable(tiles, entity->moving_to_x, entity->moving_to_y + 1);
            char rP = tile_right_pathfindable(tiles, entity->moving_to_x, entity->moving_to_y);
            char ruP = tile_right_pathfindable(tiles, entity->moving_to_x, entity->moving_to_y - 1);
            char rdP = tile_right_pathfindable(tiles, entity->moving_to_x, entity->moving_to_y + 1);
            char uP = tile_up_pathfindable(tiles, entity->moving_to_x, entity->moving_to_y);
            char dP = tile_down_pathfindable(tiles, entity->moving_to_x, entity->moving_to_y);
            switch (entity->main_dir)
            {
            // Up
            case 0:
                if ((-x_dif > -y_dif) && luP && lP) // Too far on one side
                {
                    // Move left
                    dir = 3;
                    entity->moving_to_x--;
                }
                else if ((x_dif > -y_dif) && ruP && rP) // Too far on one side
                {
                    // Move right
                    dir = 1;
                    entity->moving_to_x++;
                }
                else if (uP)
                {
                    // Move up
                    dir = 0;
                    entity->moving_to_y--;
                }
                else if (x_dif > 0)
                {
                    int dist = tile_found_path(1, 0, 0, -1, tiles, entity->moving_to_x, entity->moving_to_y);
                    // Try to go right
                    if (dist)
                    {
                        // Move right
                        dir = 1;
                        entity->moving_to_x += dist;
                    }
                    else
                    {
                        // Move left
                        dir = 3;
                        entity->moving_to_x--;
                    }
                }
                else
                {
                    int dist = tile_found_path(-1, 0, 0, -1, tiles, entity->moving_to_x, entity->moving_to_y);
                    // Try to go left
                    if (dist)
                    {
                        // Move left
                        dir = 3;
                        entity->moving_to_x -= dist;
                    }
                    else
                    {
                        // Move right
                        dir = 1;
                        entity->moving_to_x++;
                    }
                }
                break;
            // Right
            case 1:
                if ((-y_dif > x_dif) && ruP && uP) // Too far on one side
                {
                    // Move up
                    dir = 0;
                    entity->moving_to_y--;
                }
                else if ((y_dif > x_dif) && rdP && dP) // Too far on one side
                {
                    // Move down
                    dir = 2;
                    entity->moving_to_y++;
                }
                else if (rP)
                {
                    // Move right
                    dir = 1;
                    entity->moving_to_x++;
                }
                else if (y_dif < 0)
                {
                    int dist = tile_found_path(0, -1, 1, 0, tiles, entity->moving_to_x, entity->moving_to_y);
                    // Try to go up
                    if (dist)
                    {
                        // Move up
                        dir = 0;
                        entity->moving_to_y -= dist;
                    }
                    else
                    {
                        // Move down
                        dir = 2;
                        entity->moving_to_y++;
                    }
                }
                else
                {
                    int dist = tile_found_path(0, 1, 1, 0, tiles, entity->moving_to_x, entity->moving_to_y);
                    // Try to go down
                    if (dist)
                    {
                        // Move down
                        dir = 2;
                        entity->moving_to_y += dist;
                    }
                    else
                    {
                        // Move up
                        dir = 0;
                        entity->moving_to_y--;
                    }
                }
                break;
            // Down
            case 2:
                if ((-x_dif > y_dif) && ldP && lP) // Too far on one side
                {
                    // Move left
                    dir = 3;
                    entity->moving_to_x--;
                }
                else if ((x_dif > y_dif) && rdP && rP) // Too far on one side
                {
                    // Move right
                    dir = 1;
                    entity->moving_to_x++;
                }
                else if (dP)
                {
                    // Move down
                    dir = 2;
                    entity->moving_to_y++;
                }
                else if (x_dif > 0)
                {
                    int dist = tile_found_path(1, 0, 0, 1, tiles, entity->moving_to_x, entity->moving_to_y);
                    // Try to go right
                    if (dist)
                    {
                        // Move right
                        dir = 1;
                        entity->moving_to_x += dist;
                    }
                    else
                    {
                        // Move left
                        dir = 3;
                        entity->moving_to_x--;
                    }
                }
                else
                {
                    int dist = tile_found_path(-1, 0, 0, 1, tiles, entity->moving_to_x, entity->moving_to_y);
                    // Try to go left
                    if (dist)
                    {
                        // Move left
                        dir = 3;
                        entity->moving_to_x -= dist;
                    }
                    else
                    {
                        // Move right
                        dir = 1;
                        entity->moving_to_x++;
                    }
                }
                break;
            // Left
            case 3:
                if ((-y_dif > -x_dif) && luP && uP) // Too far on one side
                {
                    // Move up
                    dir = 0;
                    entity->moving_to_y--;
                }
                else if ((y_dif > -x_dif) && ldP && dP) // Too far on one side
                {
                    // Move down
                    dir = 2;
                    entity->moving_to_y++;
                }
                else if (lP)
                {
                    // Move left
                    dir = 3;
                    entity->moving_to_x--;
                }
                else if (y_dif < 0)
                {
                    int dist = tile_found_path(0, -1, -1, 0, tiles, entity->moving_to_x, entity->moving_to_y);
                    // Try to go up
                    if (dist)
                    {
                        // Move up
                        dir = 0;
                        entity->moving_to_y -= dist;
                    }
                    else
                    {
                        // Move down
                        dir = 2;
                        entity->moving_to_y++;
                    }
                }
                else
                {
                    int dist = tile_found_path(0, 1, -1, 0, tiles, entity->moving_to_x, entity->moving_to_y);
                    // Try to go down
                    if (dist)
                    {
                        // Move down
                        dir = 2;
                        entity->moving_to_y += dist;
                    }
                    else
                    {
                        // Move up
                        dir = 0;
                        entity->moving_to_y--;
                    }
                }
                break;
            }
            entity->animation = (dir << 4) | (entity->animation & 0b1111);
            if (tile_is_attackable(tiles + (entity->moving_to_y * tY + entity->moving_to_x)))
            {
                entity->animation |= 0b1000000;
            }
        }
        else
        {
            // No room for me, skip
            return;
        }
    }
    // Attacking
    if (entity->animation & 0b1000000)
    {
        char anim = ((entity->animation & 0b1111) + 1) % 11;
        entity->animation &= 0b1110000;
        entity->animation |= anim;
        Tile *attacking = tiles + (entity->moving_to_y * tY + entity->moving_to_x);
        if (!tile_is_attackable(attacking))
        {
            entity->animation &= 0b111111;
        }
        else if ((entity->animation & 0b1111) == 7)
        {
            attacking->base_tile->health = fmax(attacking->base_tile->health - types[entity->type].damage, 0);
            if (attacking->base_tile->health == 0)
            {
                tile_destroy(tiles, attacking->base_tile, tile_types);
            }
        }
    }
    else
    {
        switch (entity->animation & 0b1110000)
        {
        // Up
        case 0b000000:
            entity->y = fmax(entity->moving_to_y, entity->y - distance_per_tick);
            break;
        // Right
        case 0b010000:
            entity->x = fmin(entity->moving_to_x, entity->x + distance_per_tick);
            break;
        // Down
        case 0b100000:
            entity->y = fmin(entity->moving_to_y, entity->y + distance_per_tick);
            break;
        // Left
        case 0b110000:
            entity->x = fmax(entity->moving_to_x, entity->x - distance_per_tick);
            break;
        }
        char anim = ((entity->animation & 0b1111) + 1) % 16;
        entity->animation &= 0b1110000;
        entity->animation |= anim;
    }
}

void entity_type_free(EntityType *types)
{
    free(types->texture_attack);
    free(types->texture_running);
    free(types);
}

void entity_container_free(EntityContainer container)
{
    free(container.entities);
}
