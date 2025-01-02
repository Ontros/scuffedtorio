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
    printf("amount: %s", buffer);
    int amount = atoi(buffer);
    ScoreContainer container = {.amount = amount, .scores = (HighScore *)(malloc(sizeof(HighScore) * amount)), .texts = (Text *)(malloc(sizeof(Text) * amount))};
    printf("amount: %d\n", container.amount);
    for (int i = 0; i < amount; i++)
    {
        fgets(buffer, sizeof(buffer), scores);
        container.scores[i].pokus = atoi(buffer);
        fgets(buffer, sizeof(buffer), scores);
        container.scores[i].score = atoi(buffer);
        container.texts[i] = text_init("./data/core/fonts/TitilliumWeb-SemiBold.ttf", 24, 50);
        sprintf(container.texts[i].buffer, "Pokus %d: %d", container.scores[i].pokus, container.scores[i].score);
        text_create_with_pos(renderer, container.texts + i, 0, i * 24);
    }
    fclose(scores);
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

void score_save(ScoreContainer container, int cur_score)
{
    FILE *scores = fopen("./highscore.txt", "w");
    if (!scores)
    {
        printf("Cannon open file\n");
    }
    if (!container.amount)
    {
        fprintf(scores, "1\n1\n%d\n", cur_score);
    }
    else
    {
        printf("Openned file\n");
        fprintf(scores, "%d\n", container.amount + 1);
        int offset = 0;
        for (int i = 0; i < container.amount; i++)
        {
            if (!offset && container.scores[i].score <= cur_score)
            {
                fprintf(scores, "%d\n", container.amount);
                fprintf(scores, "%d\n", cur_score);
                offset = 1;
                i--;
            }
            else
            {
                fprintf(scores, "%d\n", container.scores[i].pokus);
                fprintf(scores, "%d\n", container.scores[i].score);
            }
        }
    }
    fclose(scores);
    score_free(container);
}
