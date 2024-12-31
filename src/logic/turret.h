#pragma once
#include "../structs.h"
#include "turret.c"

void turret_tick(Tile *tiles, TileType *types, int updates, BulletList **bullet_list, BulletList **laser_list, FlameList **flame_list, EntityContainer entity_container, EntityType *entity_types, GameState *state);