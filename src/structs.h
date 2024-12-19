#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
const int tX = 16 * 16 * 4;
const int tY = 16 * 16 * 4;
// x,y - pos of top left
// size - sizeo of a tile
typedef struct Camera
{
    float x;
    float y;
    float size;
    float width;
    float height;
    int movement_x;
    int movement_y;
    float speed_factor;
    float scroll_factor;
} Camera;

typedef struct
{
    SDL_Texture *texture;
    float x_offset;
    float y_offset;
    float x_size;
    float y_size;
} TextureOffset;

typedef struct
{
    unsigned char item;
    unsigned char cost;
} Tile_Cost;

typedef struct Tile Tile;

typedef struct Tile
{
    Tile *base_tile;
    int x;
    int y;
    unsigned int flags;
    char type;
    char health;
} Tile;

typedef struct TileType
{
    SDL_Texture *texture;
    SDL_Rect *animation_rects;
    Tile_Cost *costs;
    char *name;
    unsigned int anim_tile_x;
    unsigned int anim_tile_y;
    unsigned char size_x;
    unsigned char size_y;
    unsigned char x_map;
    unsigned char y_offset;
    unsigned char animation_modulo;
    unsigned char animation_mask;
    unsigned char id;
    unsigned char textures_count;
    unsigned char max_health;
    unsigned char cost_count;
} TileType;

typedef struct KeyStates
{
    int up : 1;
    int down : 1;
    int left : 1;
    int right : 1;
    int mouse_left : 1;
    int mouse_right : 1;
} KeyStates;

typedef struct Text
{
    char *buffer;
    TTF_Font *font;
    SDL_Texture *texture;
    SDL_Surface *surface;
    SDL_Rect *rect;
} Text;