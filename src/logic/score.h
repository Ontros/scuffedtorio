#pragma once
#include "../structs.h"
#include "../rendering/ui/text.h"
#include "score.c"

ScoreContainer score_load(SDL_Renderer *renderer);
void score_free(ScoreContainer container);
void score_render(SDL_Renderer *renderer, ScoreContainer container);
void score_save(ScoreContainer container);