#pragma once

#include "def.h"
#include "expanded_list.h"
#include "tools.h"
#include "block.h"

#define BLOCK_CHUNK_SZ 20

struct Player;

typedef struct Chunk
{
    int x;
    int y;
    Block* block[BLOCK_CHUNK_SZ][BLOCK_CHUNK_SZ];
} Chunk;


typedef struct Level
{
    Gl_texture* map;
    Extended_list* layer_list;
    number_t camera_x;
    number_t camera_y;
    number_t gravity_multiplier;
    Extended_list* chunk_list;
    byte3 bgcol;
    int2 start_pos[8];
    short map_border_x1;
    short map_border_y1;
    short map_border_x2;
    short map_border_y2;

} Level;

extern Level* the_level;
extern const number_t g_default_gravity;
extern enum Rotation current_rotation;

Level* create_level();
void destroy_level(Level *lvl);
int2 rotate_position(int x, int y, enum Rotation rot, int square_size);
void set_block(Level *lvl, int x, int y, int type);
Block *get_block(Level *lvl, int x, int y);
Block *pr2_get_block(number_t x, number_t y, bool b1, bool b2);
Block *pr2_get_block_from_pos(number_t pos_x, number_t pos_y, bool b1);
void render_level(Level *lvl, number_t screen_x, number_t screen_y);
void update_level(Level *lvl);
void update_block_offsets();
int get_level_max_y();
int add_chunk_to_level(Level *lvl, int x, int y);
void render_chunk(Chunk *seg, number_t screen_x, number_t screen_y);
void update_chunk(Chunk *seg);
void update_block_offsets_on_chunk(Chunk *seg);
void set_block_on_chunk(Chunk *seg, int x, int y, int type);
Block *get_block_from_chunk(Chunk *seg, int x, int y);
void move_block(Level *lvl, int x1, int y1, int x2, int y2);
bool push_block(Level *lvl, enum Direction dir, int x, int y);
bool is_player_at(Level *lvl, int x, int y);
Level *load_pr2_level(const char *filename, bool load_art);
Block *find_teleport_of_color(int start_x, int start_y, int color);
void render_and_handle_level_gui();
