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

typedef struct BuiltTile
{
    Tile *tile;
    unsigned char health;
    char type;
} BuiltTile;

typedef struct Tile
{
    BuiltTile *base_tile;
    int x;
    int y;
    unsigned char flags;
    char ore;
    char terrain;
    char entity_occupied;
    char type;
} Tile;

typedef struct TileType
{
    SDL_Texture *texture;
    SDL_Rect *animation_rects;
    Tile_Cost *costs;
    char *name;
    unsigned int anim_tile_x;
    unsigned int anim_tile_y;
    float turret_radius;
    unsigned char size_x;
    unsigned char size_y;
    unsigned char x_count;
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

typedef struct
{
    Text text;
    SDL_Texture *texture;
    SDL_Rect rect;
    char *name;
    int count;
    int id;
} InventorySlot;

typedef struct Wave
{
    int enemies_count;
    float evolution_factor;
    int spawner_count;
} Wave;

typedef struct
{
    Wave *waves;
    InventorySlot *inventory;
    Tile *tiles;
    int concrete_radius;
    int concrete_upgrade_cost;
    int wave_current;
    int wave_count;
} GameState;

typedef struct Button Button;

typedef struct Button
{
    void (*on_click)(struct SDL_Renderer *, Button *, GameState *);
    Text text;
    SDL_Rect rect;
    SDL_Color base_color;
    SDL_Color hover_color;
} Button;

typedef struct ButtonContainer
{
    Button *buttons;
    int count;
} ButtonContainer;

typedef struct
{
    SDL_Texture *texture[4];
    SDL_Rect *animation_rects;
    char animation_modulo;
    char animation_mask;
} EntityTexture;

typedef struct
{
    EntityTexture *texture_running;
    EntityTexture *texture_attack;
    float size;
    float offset;
    float max_health;
    char damage;
} EntityType;

// animation - TRRAAAA
// T - type (0 - running)
// R - rotation 0 - up, 1 - right, 2 - down, 3 - left
// A - animation(run 0-15; attack 0-10)
typedef struct
{
    float x;
    float y;
    int target_x;
    int target_y;
    int moving_to_x;
    int moving_to_y;
    float health;
    char type;
    unsigned char animation;
    char is_dead;
    char main_dir;
} Entity;

typedef struct
{
    Entity *entities;
    int amount;
    int spawned;
} EntityContainer;

typedef struct
{
    int *spawner_indecies;
    int amount;
} SpawnerContainer;

typedef struct LaserList LaserList;

typedef struct LaserList
{
    LaserList *next;
    float x_start;
    float y_start;
    float x_end;
    float y_end;
} LaserList;