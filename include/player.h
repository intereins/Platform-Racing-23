#pragma once

#include "gl_texture.h"
#include "expanded_list.h"
#include "def.h"
#include "items.h"

#define HAT_COUNT 15

struct Block;

enum
{
    HAT_PROPELLER,
    HAT_SANTA,
    HAT_TOP,
    HAT_PARTY,
    HAT_CHEESE,
    HAT_MOON,
    HAT_ITEM,
    HAT_MINESWEEPER,
    HAT_COWBOY
};

enum Player_state
{
    PLAYER_STATE_JUMP,
    PLAYER_STATE_SUPER_JUMP,
    PLAYER_STATE_RUN,
    PLAYER_STATE_CRAWL_WALK,
    PLAYER_STATE_CRAWL,
    PLAYER_STATE_STAND,
    PLAYER_STATE_SWIM,
    PLAYER_STATE_BUMPED,
    PLAYER_STATE_FROZEN_SOLID,
};

enum Player_mode
{
    PLAYER_MODE_WATER,
    PLAYER_MODE_LAND,
    PLAYER_MODE_FREEZE,
    PLAYER_MODE_HURT,
    PLAYER_MODE_SQUASHED
};

typedef struct Player
{
    Gl_texture* tex_head;
	Gl_texture* tex_body;
	Gl_texture* tex_foot;

    int feet_counter;
    int feet_sign;

    number_t x;
    number_t y;
    number_t vel_x;
    number_t vel_y;
    enum Player_state state;
    enum Player_mode mode;
    enum Item held_item;
    enum Item second_item;
    short item_count;
    short second_item_count;
    short speed_burst_timer;
    int safe_x;
    int safe_y;
    int hats;

    number_t friction;
    number_t var_24;
    number_t var_157;
    number_t var_147;
    number_t var_150;
    number_t var_189;
    number_t var_240;
    number_t var_325;
    number_t var_523;
    number_t var_524;
    number_t var_599;
    number_t max_vel_x;
    number_t accel;
    number_t super_jump;
    number_t const_12;
    int var_407;
    int var_366;
    bool var_281;
    bool frozen_solid;
    bool crawling;
    bool on_ground;
    bool direction;
    bool item_lock;
    float rotating;

    bool up;
    bool down;
    bool left;
    bool right;
    bool ctrl;
    byte rot;

    struct Block* var_630;
    struct Block* var_469;
    struct Block* var_657;
    struct Block* var_329;
    struct Block* var_658;
    struct Block* var_296;
    struct Block* var_654;
    struct Block* var_262;
    struct Block* var_631;
    struct Block* var_306;
    struct Block* var_297;

    int speed_stat;
    int accel_stat;
    int jumpn_stat;
    enum Item cur_item;
    bool item_is_used;
} Player;

typedef struct Hat
{
    Object object;
    float x;
    float y;
    float vel_x;
    float vel_y;
    byte type;
} Hat;

extern Player the_player;

void player_init(Player *pl);
void player_set_stats(Player *pl, int param1, int param2, int param3, bool param4);
void player_set_mode(Player *pl, enum Player_mode mode);
void player_get_keys_from_replay_frame(Player *pl, Extended_list *replay, int frame);
void player_update_keys(Player *pl);
void player_update(Player *pl);
void player_change_state(Player *pl, enum Player_state state);
bool player_has_hat(Player *pl, int hat);
void rotate_player(Player *pl, enum Rotation rot);
void player_render(Player *pl, number_t screen_x, number_t screen_y);
void player_water_go(Player *pl);
void player_land_go(Player *pl);
void player_handle_collisions(Player* pl);
void render_player(Player* pl, float x_screen, float y_screen);
void player_set_neighboring_blocks(Player *pl);

void player_return_to_safe(Player *pl);

void player_hit(Player *pl, number_t param1, number_t param2);
