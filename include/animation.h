#pragma once

#include "gl_texture.h"

struct Player;

typedef struct Anim_data
{
	Gl_texture** tex_array;
	int frame_count;
} Anim_data;

typedef struct Anim_object
{
	Anim_data* data;
	int counter;
	float x, y;
	float w, h;	
	bool screen_pos;
	bool inverted;
} Anim_object;

extern Anim_data* anmd_teleport;
extern Anim_data* anmd_sword;
extern Anim_data* anmd_explode;

void init_animations();
Anim_object *create_anim_obj(Anim_data *data, float x, float y, bool inverted, bool screen_pos);
Anim_data *create_anim(const char *folder, int frame_count);
void render_anim(Anim_object *obj, float x, float y);
void render_and_handle_anims();
void start_anim(Anim_data *data, float x, float y, bool inverted, bool screen_pos);
