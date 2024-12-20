#include "text.h"

Text text_init(char *font_name, int font_size, int buffer_size)
{
    return (Text){
        .buffer = buffer_size == 0 ? NULL : malloc(buffer_size),
        .font = TTF_OpenFont(font_name, font_size),
        .rect = {0, 0, 0, 0},
        .surface = NULL,
        .texture = NULL};
}

void text_create(SDL_Renderer *renderer, Text *text, int x, int y)
{
    if (text->texture)
    {
        SDL_FreeSurface(text->surface);
        SDL_DestroyTexture(text->texture);
    }
    text->surface = TTF_RenderText_Solid(text->font, text->buffer, (SDL_Color){255, 255, 255, 255});
    text->texture = SDL_CreateTextureFromSurface(renderer, text->surface);
    text->rect = (SDL_Rect){x, y, text->surface->w, text->surface->h};
}

// TODO: inline
void text_render(SDL_Renderer *renderer, Text text)
{
    if (text.texture)
    {
        SDL_RenderCopy(renderer, text.texture, NULL, &text.rect);
    }
}

void text_free(Text text)
{
    if (text.buffer)
    {
        free(text.buffer);
    }
    if (text.font)
    {
        TTF_CloseFont(text.font);
    }
    if (text.surface)
    {
        SDL_FreeSurface(text.surface);
    }
    if (text.texture)
    {
        SDL_DestroyTexture(text.texture);
    }
}