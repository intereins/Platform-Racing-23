#pragma once

#include "gl_texture.h"
#include "pointer_list.h"
#include "level.h"

#define ART_CHUNK_SZ   360
#define ART_CHUNK_W (ART_CHUNK_SZ)
#define ART_CHUNK_H (ART_CHUNK_SZ)

#define ART_CHUNK_TABLE_SZ (2000*30/ART_CHUNK_SZ)

struct Player;

typedef struct Art_chunk Art_chunk;
typedef struct Art_layer Art_layer;

struct Art_chunk
{
	Gl_texture* tex;
	int x, y;
	bool needs_update;
	bool px_locked[ART_CHUNK_SZ][ART_CHUNK_SZ];
};

struct Art_layer
{
	char name[16];
	Pointer_list* art_chunk_list;
	Art_chunk* art_chunk_table[ART_CHUNK_TABLE_SZ][ART_CHUNK_TABLE_SZ];
	float depth;
	float alpha;
	bool over_blocks;
};

extern Gl_texture* bg_desert;
extern Gl_texture* bg_industrial;
extern Gl_texture* bg_jungle;
extern Gl_texture* bg_underwater;
extern Gl_texture* bg_space;
extern float pr3_multiplier;

void init_level_art(Level* lvl);
void init_backgrounds();
void render_background();
void render_art_chunk_screen_pos(Art_chunk* chunk, float x, float y, byte rot);
void render_level_art(Level* lvl, struct Player* pl, float offset_x, float offset_y);
Art_chunk* get_art_chunk_px_pos(int x, int y);
void draw_pixel(int x, int y, byte r, byte g, byte b, byte a, Art_layer* layer);
void draw_square(int x, int y, int size_px, byte r, byte g, byte b, byte a, Art_layer* layer);
void update_needing_art_chunks(Level* lvl);
void draw_circle(int x, int y, int size_px, byte r, byte g, byte b, byte a, Art_layer* layer);
void draw_circle_line(int x1, int y1, int x2, int y2, int size_px, byte r, byte g, byte b, byte a, Art_layer* layer);
void load_art_from_file(Level *lvl, FILE *file, int file_pos);
void load_block_art(int id, const char *lvl_title);
void erase_circle_on_block(int x, int y, int size_px, byte r, byte g, byte b, byte a, Art_layer* layer);
void erase_circle(int x, int y, int size_px, byte a, Art_layer* layer);
rgba_t get_pixel(int x, int y, Art_layer* layer);
void draw_circle_with_alpha(int x, int y, int size_px, byte r, byte g, byte b, byte a, Art_layer* layer);
void draw_circle_line_with_alpha(int x1, int y1, int x2, int y2, int size_px, byte r, byte g, byte b, byte a, Art_layer* layer);
void erase_circle_with_alpha(int x, int y, int size_px, byte a, Art_layer* layer);
void erase_circle_line_with_alpha(int x1, int y1, int x2, int y2, int size_px, byte a, Art_layer* layer);
void save_all_art_chunks_to_files();
void unlock_pixels(Art_layer* layer);
Art_layer* add_layer(Level* lvl, const char* name, float depth, float alpha);
void unlock_block_image_pixels();
