#include "button.h"

Button button_init(SDL_Renderer *renderer, const char *text_inp, int font_size, SDL_Rect rect, void *on_click)
{
    Text text = text_init("../data/core/fonts/TitilliumWeb-SemiBold.ttf", font_size, 50);
    memcpy(text.buffer, text_inp, strlen(text_inp));
    text_create_with_pos(renderer, &text, rect.x, rect.y);
    return (Button){
        .hover_color = (SDL_Color){55, 55, 55, 255},
        .base_color = (SDL_Color){75, 75, 75, 255},
        .on_click = on_click,
        .rect = rect,
        .text = text};
}

void button_change_text(SDL_Renderer *renderer, Button *button, const char *text)
{
    memcpy(button->text.buffer, text, strlen(text));
    text_create(renderer, &button->text);
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
