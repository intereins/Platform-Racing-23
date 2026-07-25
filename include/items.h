#pragma once

#include "gl_texture.h"
#include "objects.h"

#define ITEM_COUNT 10

struct Player;

enum Item
{
    ITEM_NONE = -1,
    ITEM_SPEED,
    ITEM_SJUMP,
    ITEM_GUN,
    ITEM_TELEPORT,
    ITEM_JETPACK,
    ITEM_SWORD,
    ITEM_ICEWAVE,
    ITEM_LIGHTNING,
    ITEM_MINE,
    ITEM_BLOCK
};

typedef struct Placed_mine
{
	int x;
	int y;
    int block_id;
	time_t time_start;
} Placed_mine;

typedef struct Bullet
{
    Object object;
    float x;
    float y;
    float vel_x;
    float vel_y;
} Bullet;

extern Gl_texture* item_textures[ITEM_COUNT];
extern Gl_texture* tex_laser;
extern Gl_texture* tex_laser_light;

void init_items();

byte get_item_by_name(const char *name);
void give_item_to_player(struct Player *pl, enum Item item);
void give_random_item_to_player(struct Player *pl, int allowed_items);
void render_item_gui(struct Player *pl);
bool use_item_portable_block(struct Player *pl, int block_id);
Bullet *create_bullet(float x, float y, float vel_x, float vel_y);
void use_item(struct Player *pl);
void handle_placed_mines();
void update_bullet(Bullet *bullet);
void render_bullet(Bullet *bullet);
void render_placed_mines(float offset_x, float offset_y);
