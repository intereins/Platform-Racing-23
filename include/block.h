#pragma once

#include "def.h"
#include "tools.h"
#include "gl_texture.h"

#define BLOCK_W 30
#define BLOCK_H 30

#define BL_BASIC_1    1
#define BL_BASIC_2    2
#define BL_BASIC_3    3
#define BL_BASIC_4    4
#define BL_BRICKS     5
#define BL_FINISH     6
#define BL_ICE        7
#define BL_ITEM       8
#define BL_ITEM_INF   9
#define BL_ARROW_LEFT  10
#define BL_ARROW_RIGHT 11
#define BL_ARROW_UP    12
#define BL_ARROW_DOWN  13
#define BL_TELEPORT   14
#define BL_MINE       15
#define BL_CRUMBLE    16
#define BL_VANISH     17
#define BL_MOVE       18
#define BL_WATER      19
#define BL_ROT_R      20
#define BL_ROT_L      21
#define BL_PUSH       22
#define BL_STAT_INC   23
#define BL_STAT_DEC   24
#define BL_STAT_CUST  25
#define BL_NET        26
#define BL_HEART      27
#define BL_CLOCK      28
#define BL_START      29
#define BL_BOUNCE     30
#define BL_CHANGE     31
#define BL_1WAY_UP    32
#define BL_1WAY_DOWN    33
#define BL_1WAY_LEFT    34
#define BL_1WAY_RIGHT    35
#define BL_PUSH_HOR    36


struct Player;

enum Direction
{
    DIR_UP = 0,
    DIR_RIGHT = 1,
    DIR_DOWN = 2,
    DIR_LEFT = 3
};

enum Block_side
{
    SIDE_TOP = 0,
    SIDE_RIGHT = 1,
    SIDE_BOTTOM = 2,
    SIDE_LEFT = 3,
    SIDE_BUMP
};

enum Block_type
{
    BT_ACTIVE,
    BT_INACTIVE,
    BT_WATER,
    BT_NET,
    BT_IMPERVIOUS,
    BT_START
};

enum Block_action
{
    BA_ACTIVE,
    BA_INACTIVE,
    BA_ICE,
    BA_BOOST_0,
    BA_BOOST_90,
    BA_BOOST_180,
    BA_BOOST_270,
    BA_PUSH,
    BA_VANISH,
    BA_SHATTER,
    BA_CRUMBLE,
    BA_EXPLODE,
    BA_ROT_90,
    BA_ROT_180,
    BA_ROT_270,
    BA_STAT_INC,
    BA_STAT_DEC,
    BA_ITEM,
    BA_BOUNCE,
    BA_TELEPORT
};

enum
{
    MOVE_UP,
    MOVE_RIGHT,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_WAIT,
    MOVE_RANDOM,
    MOVE_RETURN
};

enum
{
    IR_NONE,
    IR_PASS,
    IR_SHATTER,
    IR_VANISH,
    IR_PUSH,
    IR_EXPLODE
};

typedef struct Block_info
{
    Gl_texture* texture;
    byte* move_pattern;
    int allowed_items;
    short move_pattern_len;
    short move_delay;
    byte type;
    byte action_top;
    byte action_bottom;
    byte action_left;
    byte action_right;
    byte action_bump;
    byte gun_reaction;
    byte sword_reaction;
    bool can_vanish;
    bool can_move;
    bool safe;
    rgb_t avg_col;
} Block_info;

typedef struct Move_info
{
    byte* pattern;
    int pattern_len;
    short frequency;
} Move_info;

typedef struct Block
{
    number_t pos_x;
    number_t pos_y;
    int last_update;
    int chunk_x;
    int chunk_y;
    int block_code;
    int size;
    int x;
    int y;
    int color;

    float vel_y;
    float offset_y;

    short move_counter;
    short vanish_timer;
    short org_x;
    short org_y;
    byte move_counter2;
    ibyte vanish_timer_sign;
    byte vanish_len1;
    byte vanish_len2;
    bool can_vanish;

    short freeze_timer;
    short move_timer;

    ibyte life;
    byte item_count;

    byte teleport_timer;

    bool used;
    bool safe;
    bool removed;
    bool active;
    bool var_490;
    bool frozen;

} Block;

extern Block_info* block_table;
extern int block_count;

int get_action_id(const char *action);
void load_blocks(const char *filename);
enum Block_type get_block_type(Block *block);
int convert_pr2old_id_to_pr4(int id);
Gl_texture *get_block_texture_by_id(int id);
void block_render(Block *block, number_t x, number_t y, float rot);
bool block_is_active(Block *block, struct Player *pl);
double2 block_method_18(Block *block, number_t rotation);
bool block_is_fragile(int id);
void block_on_stand(Block *block, struct Player *pl);
void block_on_left_hit(Block *block, struct Player *pl);
void block_on_right_hit(Block *block, struct Player *pl);
void block_on_touch(Block *block, struct Player *pl);
void block_on_touch_net(Block *block, struct Player *pl);
void block_on_touch_water(Block *block, struct Player *pl);
void block_on_bump(Block *block, struct Player *pl);
void block_hit(Block *block, number_t x, number_t y);
void block_on_stand_boost(Block *block, struct Player *pl);
void block_on_bump_boost(Block *block, struct Player *pl, int boost_dir);
void block_on_left_hit_boost(Block *block, struct Player *pl);
void block_on_right_hit_boost(Block *block, struct Player *pl);
byte get_block_action(struct Player *pl, Block *block, enum Block_side side, int lvl_rot);
byte get_block_action_from_id(int id, int side);
void update_block_offset(Block *block);
void block_update(Block *block);
void use_teleport_block(Block *block, struct Player *pl);
void block_vanish(Block *block);
void block_throw_pieces(Block *block, int count);
void block_break(Block *block);
void block_explode(Block* block);
void hit_block_with_sword(Block *block);
void block_on_touch_mine(Block *block, struct Player *pl);
number_t get_block_pos_x(Block *block);
number_t get_block_pos_y(Block *block);
int get_block_x(Block *block);
int get_block_y(Block *block);
