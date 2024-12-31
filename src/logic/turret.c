#include "turret.h"

void turret_tick(Tile *tiles, TileType *types, int updates, BulletList **bullet_list, BulletList **laser_list, FlameList **flame_list, EntityContainer entity_container, EntityType *entity_types, GameState *state)
{
    bullet_list_free(*laser_list);
    *laser_list = NULL;
    bullet_list_free(*bullet_list);
    *bullet_list = NULL;
    *flame_list = flame_list_tick(*flame_list, entity_container, entity_types);
    for (int i = 0; i < tX * tY; i++)
    {
        if (tiles[i].base_tile)
        {
            switch (tiles[i].type)
            {
                // Gun turret
            case 2:
                if ((updates % 4) == 0)
                {
                    Entity *closest = NULL;
                    float closest_dist = tX * tY * 4; // 4*just to be safe
                    for (int j = 0; j < entity_container.amount; j++)
                    {
                        if (entity_container.entities[j].is_dead == 0)
                        {
                            float dist = pow((float)tiles[i].x - entity_container.entities[j].x, 2) + pow((float)tiles[i].y - entity_container.entities[j].y, 2);
                            if (dist < pow(types[2].turret_radius, 2) && dist < closest_dist)
                            {
                                closest_dist = dist;
                                closest = entity_container.entities + j;
                            }
                        }
                    }
                    if (closest)
                    {
                        closest->health -= 1.0f;
                        tiles[i].flags = (unsigned char)(atan2c((float)tiles[i].y - closest->y, closest->x - (float)tiles[i].x) * 128);
                        // Put new line at start
                        BulletList *new_list = (BulletList *)(malloc(sizeof(BulletList)));
                        new_list->next = *bullet_list;
                        *bullet_list = new_list;
                        (*bullet_list)->x_start = (float)tiles[i].x + 1;
                        (*bullet_list)->y_start = (float)tiles[i].y + 1;
                        (*bullet_list)->x_end = closest->x;
                        (*bullet_list)->y_end = closest->y;
                        if (closest->health <= 0)
                        {
                            closest->is_dead = 1;
                            state->entity_container.alive--;
                            state->inventory[5].count += pow(closest->type + 1, 2);
                        }
                    }
                }
                break;
            // Laser turret
            case 3:
                Entity *closest = NULL;
                float closest_dist = tX * tY * 4; // 4*just to be safe
                for (int j = 0; j < entity_container.amount; j++)
                {
                    if (entity_container.entities[j].is_dead == 0)
                    {
                        float dist = pow((float)tiles[i].x - entity_container.entities[j].x, 2) + pow((float)tiles[i].y - entity_container.entities[j].y, 2);
                        if (dist < pow(types[3].turret_radius, 2) && dist < closest_dist)
                        {
                            closest_dist = dist;
                            closest = entity_container.entities + j;
                        }
                    }
                }
                if (closest)
                {
                    closest->health -= 1.0f;
                    tiles[i].flags = (unsigned char)(atan2c((float)tiles[i].y - closest->y, closest->x - (float)tiles[i].x) * 64);
                    // Put new line at start
                    BulletList *new_list = (BulletList *)(malloc(sizeof(BulletList)));
                    new_list->next = *laser_list;
                    *laser_list = new_list;
                    (*laser_list)->x_start = (float)tiles[i].x + 1;
                    (*laser_list)->y_start = (float)tiles[i].y + 1;
                    (*laser_list)->x_end = closest->x;
                    (*laser_list)->y_end = closest->y;
                    if (closest->health <= 0)
                    {
                        closest->is_dead = 1;
                        state->entity_container.alive--;
                        state->inventory[5].count += pow(closest->type + 1, 2);
                    }
                }
                break;
                // Flame turret
            case 6:
                if ((updates % 8) == 0)
                {
                    Entity *closest = NULL;
                    float closest_dist = tX * tY * 4; // 4*just to be safe
                    for (int j = 0; j < entity_container.amount; j++)
                    {
                        if (entity_container.entities[j].is_dead == 0)
                        {
                            float dist = pow((float)tiles[i].x - entity_container.entities[j].x, 2) + pow((float)tiles[i].y - entity_container.entities[j].y, 2);
                            if (dist < pow(types[6].turret_radius, 2) && dist < closest_dist)
                            {
                                closest_dist = dist;
                                closest = entity_container.entities + j;
                            }
                        }
                    }
                    if (closest)
                    {
                        closest->health -= 1.0f;
                        tiles[i].flags = (unsigned char)(atan2c(((float)tiles[i].y - closest->y), -(closest->x - (float)tiles[i].x)) * 64);
                        // Put new line at start
                        FlameList *new_list = (FlameList *)(malloc(sizeof(FlameList)));
                        new_list->next = *flame_list;
                        *flame_list = new_list;
                        (*flame_list)->x = (float)tiles[i].x + 1;
                        (*flame_list)->y = (float)tiles[i].y + 1;
                        (*flame_list)->x_end = closest->x;
                        (*flame_list)->y_end = closest->y;
                        (*flame_list)->live_left = 180;
                        if (closest->health <= 0)
                        {
                            closest->is_dead = 1;
                            state->entity_container.alive--;
                            state->inventory[5].count += pow(closest->type + 1, 2);
                        }
                    }
                }
                break;

            default:
                break;
            }
        }
    }
}