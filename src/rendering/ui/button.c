#include "button.h"

Button button_init(SDL_Renderer *renderer, const char *text_inp, int font_size, SDL_Rect rect, void (*on_click)(void *, void *, void *))
{
    Text text = text_init("./data/core/fonts/TitilliumWeb-SemiBold.ttf", font_size, 50);
    memcpy(text.buffer, text_inp, strlen(text_inp));
    text_create_with_pos(renderer, &text, rect.x + 10, rect.y);
    return (Button){
        .hover_color = (SDL_Color){55, 55, 55, 255},
        .base_color = (SDL_Color){75, 75, 75, 255},
        .on_click = on_click,
        .rect = rect,
        .text = text};
}

inline static char is_mouse_in_rect(SDL_Rect rect, int x, int y)
{
    return (x > rect.x) && (x < rect.x + rect.w) && (y > rect.y) && (y < rect.y + rect.h);
}

void button_render(SDL_Renderer *renderer, Button *button, int mouse_x, int mouse_y)
{
    if (is_mouse_in_rect(button->rect, mouse_x, mouse_y))
        SDL_SetRenderDrawColor(renderer, button->hover_color.r, button->hover_color.g, button->hover_color.b, button->hover_color.a);
    else
        SDL_SetRenderDrawColor(renderer, button->base_color.r, button->base_color.g, button->base_color.b, button->base_color.a);
    SDL_RenderFillRect(renderer, &button->rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &button->rect);
    text_render(renderer, button->text);
}

void button_expand_concrete_click(void *v0, void *v1, void *v2)
{
    SDL_Renderer *renderer = (SDL_Renderer *)v0;
    Button *button = (Button *)v1;
    GameState *state = (GameState *)v2;
    if (state->inventory[5].count >= state->concrete_upgrade_cost)
    {
        state->inventory[5].count -= state->concrete_upgrade_cost;
        state->concrete_radius += 16;
        state->concrete_upgrade_cost += state->concrete_radius;
        tile_update_concrete(state->tiles, state->concrete_radius);
        sprintf(button->text.buffer, "Expand concrete (%d coins)", state->concrete_upgrade_cost);
        text_create(renderer, &button->text);
    }
}

void button_next_wave_click(void *v0, void *v1, void *v2)
{
    SDL_Renderer *renderer = (SDL_Renderer *)v0;
    GameState *state = (GameState *)v2;
    state->wave_current++;
    state->is_wave_running = 1;
    if (state->is_infinite)
    {
        state->wave = (Wave){.enemies_count = state->wave_current * 10, .evolution_factor = fmin(2, state->wave_current / 5), .spawner_count = state->wave_current * 15};
    }
    else
    {
        state->wave = (Wave[3]){{10, 0, 5}, {100, 1, 50}, {500, 2, 250}}[state->wave_current - 1];
    }
    state->spawner_container = spawner_spawn(state->tiles, *state, state->types[5]);
    state->entity_container = entity_container_create(state->wave.enemies_count);
    for (int i = 0; i < state->wave.enemies_count; i++)
    {
        entity_spawn(state->entity_container.entities + i, state->tiles, state->spawner_container, 0, state->entity_types + (int)state->wave.evolution_factor, *state);
    }
}

char button_container_click(SDL_Renderer *renderer, GameState *state, ButtonContainer container, int mouse_x, int mouse_y)
{
    int ret = 0;
    for (int i = 0; i < container.count; i++)
    {
        if (is_mouse_in_rect(container.buttons[i].rect, mouse_x, mouse_y))
        {
            container.buttons[i].on_click(renderer, container.buttons + i, state);
            ret = 1;
        }
    }
    return ret;
}

void button_container_render(SDL_Renderer *renderer, ButtonContainer container, int mouse_x, int mouse_y)
{
    for (int i = 0; i < container.count; i++)
    {
        button_render(renderer, container.buttons + i, mouse_x, mouse_y);
    }
}

ButtonContainer button_container_in_game_create(SDL_Renderer *renderer, GameState state)
{
    int button_count = 2;
    ButtonContainer buttons = (ButtonContainer){
        .buttons = (Button *)(malloc(sizeof(Button) * button_count)),
        .count = button_count};
    buttons.buttons[0] = button_init(renderer, "Expand concrete (100 coins)", 24, (SDL_Rect){200, 0, 350, 50}, button_expand_concrete_click);
    buttons.buttons[1] = button_init(renderer, "Start Wave", 24, (SDL_Rect){600, 0, 150, 50}, button_next_wave_click);
    return buttons;
}

void button_container_free(ButtonContainer container)
{
    for (int i = 0; i < container.count; i++)
    {
        text_free(container.buttons[i].text);
    }
    free(container.buttons);
}
