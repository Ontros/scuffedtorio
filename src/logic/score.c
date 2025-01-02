#include "score.h"

ScoreContainer score_load(SDL_Renderer *renderer)
{
    FILE *scores = fopen("./highscore.txt", "r");
    if (!scores)
    {
        return (ScoreContainer){.amount = 0, .scores = NULL, .texts = NULL};
    }
    char buffer[50];
    fgets(buffer, sizeof(buffer), scores);
    int amount = atoi(buffer);
    ScoreContainer container = {.amount = amount, .scores = (HighScore *)(malloc(sizeof(HighScore) * amount)), .texts = (Text *)(malloc(sizeof(Text) * amount))};
    for (int i = 0; i < amount; i++)
    {
        fgets(buffer, sizeof(buffer), scores);
        container.scores[i].pokus = atoi(buffer);
        fgets(buffer, sizeof(buffer), scores);
        container.scores[i].score = atoi(buffer);
        printf("%d %d %d\n", i, container.scores[i].pokus, container.scores[i].score);
        container.texts[i] = text_init("./data/core/fonts/TitilliumWeb-SemiBold.ttf", 24, 50);
        sprintf(container.texts[i].buffer, "Pokus %d: %d", container.scores[i].pokus, container.scores[i].score);
        text_create_with_pos(renderer, container.texts + i, 0, i * 24);
    }
    return container;
}

void score_free(ScoreContainer container)
{
    for (int i = 0; i < container.amount; i++)
    {
        text_free(container.texts[i]);
    }
    if (container.amount)
    {
        free(container.texts);
        free(container.scores);
    }
}

void score_render(SDL_Renderer *renderer, ScoreContainer container)
{
    for (int i = 0; i < container.amount; i++)
    {
        text_render(renderer, container.texts[i]);
    }
}

void score_save(ScoreContainer container)
{
}
