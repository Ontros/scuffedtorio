#pragma once
const int tX = 16 * 16 * 4;
const int tY = 16 * 16 * 4;
// x,y - pos of top left
// size - sizeo of a tile
typedef struct Camera
{
    float x;
    float y;
    float size;
} Camera;

typedef struct Tile Tile;

typedef struct Tile
{
    Tile *base_tile;
    int x;
    int y;
    unsigned int flags;
    char type;
} Tile;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Rect SDL_Rect;

typedef struct TileType
{
    SDL_Texture *texture;
    SDL_Rect *animation_rects;
    unsigned int anim_tile_x;
    unsigned int anim_tile_y;
    unsigned char size_x;
    unsigned char size_y;
    unsigned char x_map;
    unsigned char y_offset;
    unsigned char animation_modulo;
    unsigned char animation_mask;
    unsigned char id;
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