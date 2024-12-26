#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
const int tX = 16 * 16 * 2;
const int tY = 16 * 16 * 2;
const int cX = 16 * 16;
const int cY = 16 * 16;
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
    int item;
    int count;
} Tile_Cost;

typedef struct Tile Tile;

typedef struct Tile
{
    Tile *base_tile;
    int x;
    int y;
    unsigned char flags;
    char ore;
    char terrain;
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
    SDL_Rect rect;
} Text;

typedef struct Button
{
    void (*on_click)(void);
    Text text;
    SDL_Rect rect;
    SDL_Color base_color;
    SDL_Color hover_color;
} Button;

typedef struct
{
    Text text;
    SDL_Texture *texture;
    SDL_Rect rect;
    char *name;
    int count;
    int id;
} InventorySlot;

typedef struct
{
    SDL_Texture *texture;
    SDL_Rect *animation_rects;
    float size_x;
    float size_y;
    float damage;
    float max_health;
} EntityType;

typedef struct
{
    float x;
    float y;
    float target_x;
    float target_y;
    char type;
    char animation;
    char health;
} Entity;