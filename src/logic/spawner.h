#pragma once
#include "../structs.h"
#include "tile.h"
#include "spawner.c"

SpawnerContainer spawner_spawn(Tile *tiles, GameState state, TileType spawner_type);

void spawner_free(SpawnerContainer container);