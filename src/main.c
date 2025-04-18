#include "game.h"
#include "SDL2/SDL_video.h"

int main(int argc, char *argv[])
{
    Camera camera = {-cX + 4, -cY + 4, 100, 1920, 1080, 2, 2, 7, 1};
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    char running = 1;

    MenuState state = {.game_running = 0, .mode = 0, .wave_build = 1};

    if (sdl_init(camera, &window, &renderer))
    {
        return 1;
    }
    SDL_Texture *scufftorio_logo = IMG_LoadTexture(renderer, "./images/scufftorio.png");
    if (!scufftorio_logo)
    {
        printf("Logo not found\n");
        return 1;
    }
    SDL_SetTextureBlendMode(scufftorio_logo, SDL_BLENDMODE_BLEND);
    SDL_Texture *background = IMG_LoadTexture(renderer, "./data/core/graphics/background-image.jpg");
    if (!background)
    {
        printf("Background not found\n");
        return 1;
    }
    SDL_SetTextureBlendMode(background, SDL_BLENDMODE_BLEND);
    ButtonContainer container = button_container_menu_create(renderer);
    button_container_center(container, camera.width);
    Text result_text = text_init("./data/core/fonts/TitilliumWeb-SemiBold.ttf", 48, 50);
    sprintf(result_text.buffer, " ");
    text_create_with_pos(renderer, &result_text, 0, 450);
    int mouse_x, mouse_y;

    ScoreContainer score_container = score_load(renderer);

    while (running)
    {
        if (state.game_running == 1)
        {
            // Game creates a new window to avoid a memory leak from not destroying renderer
            SDL_HideWindow(window);
            GameResult result = game(&camera, state);
            if (result.won)
            {
                sprintf(result_text.buffer, "You won, score: %d", result.score);
            }
            else
            {
                sprintf(result_text.buffer, "You lost, score: %d", result.score);
            }
            text_create_with_pos(renderer, &result_text, (camera.width - 300) / 2, 450);
            if (result.score >= 0)
            {
                printf("Writing score: %d\n", result.score);
                score_save(score_container, result.score);
                score_container = score_load(renderer);
            }
            SDL_ShowWindow(window);
            state.game_running = 0;
        }
        else
        {
            SDL_GetMouseState(&mouse_x, &mouse_y);
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    running = 0;
                }
                else if (event.type == SDL_WINDOWEVENT)
                {
                    if (event.window.type == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_RESIZED)
                    {
                        camera.width = event.window.data1;
                        camera.height = event.window.data2;
                        button_container_center(container, camera.width);
                    }
                }
                else if (event.type == SDL_MOUSEBUTTONDOWN)
                {
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        for (int i = 0; i < container.count; i++)
                        {
                            if (is_mouse_in_rect(container.buttons[i].rect, mouse_x, mouse_y))
                            {
                                container.buttons[i].on_click(&container.buttons[i], &state, renderer);
                            }
                        }
                    }
                }
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, background, NULL, &(SDL_Rect){0, 0, camera.width, camera.height});
            score_render(renderer, score_container);
            text_render(renderer, result_text);
            SDL_RenderCopy(renderer, scufftorio_logo, NULL, &(SDL_Rect){(camera.width - 972) / 2, 0, 972, 133});
            button_container_render(renderer, container, mouse_x, mouse_y);
            SDL_RenderPresent(renderer);
            SDL_Delay(17);
        }
    }

    text_free(result_text);
    score_free(score_container);
    SDL_DestroyTexture(scufftorio_logo);
    button_container_free(container);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}