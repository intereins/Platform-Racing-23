#include "animation.h"
#include "new_img_lib.h"
#include "pointer_list.h"
#include "player.h"
#include "render.h"
#include "main.h"
#include "def.h"
#include "level.h"
#include "debug.h"

Anim_data* anmd_teleport;
Anim_data* anmd_sword;
Anim_data* anmd_explode;
Pointer_list* lc_anim_list;

void init_animations()
{
	anmd_teleport = create_anim("teleport", 17);
	anmd_sword = create_anim("sword", 14);
	anmd_explode = create_anim("explode", 14);
	lc_anim_list = ptrl_create_list(10, 2);
}

Anim_object* create_anim_obj(Anim_data* data, float x, float y, bool inverted, bool screen_pos)
{
	Anim_object* obj = malloc(sizeof(Anim_object));
	obj->counter = 0;
	obj->data = data;
	obj->x = x;
	obj->y = y;
	obj->inverted = inverted;
	obj->screen_pos = screen_pos;
	return obj;
}

Anim_data* create_anim(const char* folder, int frame_count)
{
	Anim_data* ret = malloc(sizeof(Anim_object));
	ret->tex_array = calloc(frame_count, sizeof(void*));
	ret->frame_count = frame_count;
	char filename[64];
	for(int i=0; i<frame_count; i++)
	{
		sprintf(filename, "assets/anim/%s/%i.png", folder, i+1);
		ret->tex_array[i] = gtex_load_texture(filename);
	}
	return ret;
}

void render_anim(Anim_object* obj, float x, float y)
{
	x += var_x;
	y += var_y;

	float w = obj->data->tex_array[obj->counter]->img->width;
	float h = obj->data->tex_array[obj->counter]->img->height;

	if(obj->inverted) gtex_render_texture_inv(obj->data->tex_array[obj->counter], x, y, x+w, y+h);
	else gtex_render_texture(obj->data->tex_array[obj->counter], x, y, x+w, y+h);

}

void render_and_handle_anims()
{
	for(int i=0; i<lc_anim_list->elm_counter; i++)
	{
		Anim_object* obj = ptrl_get_pointer(lc_anim_list, i);
		if(obj->screen_pos)
			render_anim(obj, obj->x, obj->y);
		else
			render_anim(obj, obj->x - the_level->camera_x - 250, obj->y - the_level->camera_y - 88);

		obj->counter++;
		if(obj->counter >= obj->data->frame_count)
		{
			ptrl_remove_elm(lc_anim_list, i);
			i--;
		}
	}
}

void start_anim(Anim_data* data, float x, float y, bool inverted, bool screen_pos)
{
	Anim_object* obj = create_anim_obj(data, x, y, inverted, screen_pos);
	ptrl_add_pointer(lc_anim_list, obj, true);
}
