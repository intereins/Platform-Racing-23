#include <math.h>
#include "art.h"
#include "main.h"
#include "def.h"
#include "player.h"
#include "debug.h"

float pr3_multiplier = 1;

Gl_texture* bg_classic = NULL;
Gl_texture* bg_desert = NULL;
Gl_texture* bg_industrial = NULL;
Gl_texture* bg_jungle = NULL;
Gl_texture* bg_underwater = NULL;
Gl_texture* bg_space = NULL;

Image_bmp* block_image = NULL;
bool block_image_px_locked[40][40];

void unlock_block_image_pixels()
{
	memset(block_image_px_locked, 0, 40*40);
}

rgba_t get_pixel_from_block_image(int x, int y)
{
	return iml_get_image_pixel_rgba(block_image, x, y);
}

void clear_block_image()
{
	for(int y=0; y<40; y++)
	{
		for(int x=0; x<40; x++)
		{
			iml_set_image_pixel_rgba(block_image, x, y, 0, 0, 0, 0);
			block_image_px_locked[x][y] = false;
		}
	}
}

void init_backgrounds()
{
/* TODO (intereins#1#10/23/23): remove it from here */
	block_image = iml_create_image_struct(40, 40, 32);

	bg_desert = gtex_load_texture("bgs/desert.png");
	bg_classic = gtex_load_texture("bgs/classic.png");
	bg_industrial = gtex_load_texture("bgs/industrial.png");
	bg_jungle = gtex_load_texture("bgs/jungle.png");
	bg_underwater = gtex_load_texture("bgs/underwater.png");
	bg_space = gtex_load_texture("bgs/space.png");
}

void render_background()
{
	gtex_render_texture(bg_classic, 0, 0, 1, 1);
}

Art_layer* add_layer(Level* lvl, const char* name, float depth, float alpha)
{
	Art_layer* layer = exl_add_get_ptr(lvl->layer_list);
	layer->art_chunk_list = ptrl_create_list(5, 1.5);
	memset(layer->art_chunk_table, 0, ART_CHUNK_TABLE_SZ*ART_CHUNK_TABLE_SZ * sizeof(Art_chunk*));
	layer->depth = depth;
	layer->alpha = alpha;
	strncpy(layer->name, name, 15);
	return layer;
}

void init_level_art(Level* lvl)
{
	lvl->layer_list = exl_create(5, sizeof(Art_layer), 2);
	add_layer(lvl, "Layer 1", 1, 1);
	add_layer(lvl, "Layer 2", 1, 1);
	add_layer(lvl, "Layer 3", 0.5, 1);
	add_layer(lvl, "Layer 4", 0.25, 1);
}

void render_art_chunk_screen_pos(Art_chunk* chunk, float x, float y, byte rot)
{
	//printf("rendering %i %i at %f %f\n", chunk->x, chunk->y, x, y);
	x -= 0.5;
	y -= 0.5;

	if(chunk->tex == NULL)
	{
		return;
	}

	switch(rot)
	{
		case ROT_0: gtex_render_texture(chunk->tex, x, y, x+ART_CHUNK_SZ, y+ART_CHUNK_SZ); break;
		case ROT_90: gtex_render_texture_rot90(chunk->tex, x, y, x+ART_CHUNK_SZ, y+ART_CHUNK_SZ); break;
		case ROT_270: gtex_render_texture_rot270(chunk->tex, x, y, x+ART_CHUNK_SZ, y+ART_CHUNK_SZ); break;
	}
}

void render_level_art(Level* lvl, Player* pl, float offset_x, float offset_y) //depth is handled here
{
	if(pl->rot == ROT_0)
	{		
		for(int layer_i=lvl->layer_list->elm_count-1; layer_i>=0; layer_i--)		
		{			
			Art_layer* layer = exl_get_elm_ptr(lvl->layer_list, layer_i);
			for(int i=0; i<layer->art_chunk_list->elm_counter; i++)
			{				
				Art_chunk* chunk = ptrl_get_pointer(layer->art_chunk_list, i);				
				if(layer_i == 3)
				{
					render_art_chunk_screen_pos(chunk, chunk->x*ART_CHUNK_W - offset_x*layer->depth - 75, chunk->y*ART_CHUNK_H - offset_y*layer->depth - 58, ROT_0);
				}
				else render_art_chunk_screen_pos(chunk, chunk->x*ART_CHUNK_W - offset_x*layer->depth - 276, chunk->y*ART_CHUNK_H - offset_y*layer->depth - 201, ROT_0);				
			}
		}
	}	
}

void draw_pixel(int x, int y, byte r, byte g, byte b, byte a, Art_layer* layer)
{
	x += 275;
	y += 200;
	if(x < 0 || y < 0) return;

	Art_chunk* chunk = layer->art_chunk_table[x/ART_CHUNK_SZ][y/ART_CHUNK_SZ];
	if(chunk == NULL)
	{
		chunk = malloc(sizeof(Art_chunk));
		chunk->tex = gtex_create_texture_and_img(ART_CHUNK_SZ, ART_CHUNK_SZ, 32);
		chunk->x = x/ART_CHUNK_SZ;
		chunk->y = y/ART_CHUNK_SZ;

		ptrl_add_pointer(layer->art_chunk_list, chunk, false);
		layer->art_chunk_table[x/ART_CHUNK_SZ][y/ART_CHUNK_SZ] = chunk;
	}
	
	iml_set_image_pixel_rgba(chunk->tex->img, x%ART_CHUNK_SZ, y%ART_CHUNK_SZ, r, g, b, a);	
	chunk->needs_update = true;
}

rgba_t get_pixel(int x, int y, Art_layer* layer)
{
	x += 275;
	y += 200;

	Art_chunk* chunk = layer->art_chunk_table[x/ART_CHUNK_SZ][y/ART_CHUNK_SZ];
	if(chunk == NULL)
	{
		return (rgba_t) {0, 0, 0, 0};
	}

	rgba_t ret = iml_get_image_pixel_rgba(chunk->tex->img, x%ART_CHUNK_SZ, y%ART_CHUNK_SZ);
	return ret;
}

void draw_square(int x, int y, int size_px, byte r, byte g, byte b, byte a, Art_layer* layer)
{
	for(int ix=x-size_px; ix<=x+size_px; ix++)
	{
		for(int iy=y-size_px; iy<=y+size_px; iy++)
		{
			draw_pixel(ix, iy, r, g, b, a, layer);
		}
	}
	update_needing_art_chunks(the_level);
}

void draw_circle(int x, int y, int size_px, byte r, byte g, byte b, byte a, Art_layer* layer)
{
	for(int ix=x-size_px; ix<=x+size_px; ix++)
	{
		for(int iy=y-size_px; iy<=y+size_px; iy++)
		{
			if((ix-x)*(ix-x) + (iy-y)*(iy-y) <= size_px*size_px)
			{
				draw_pixel(ix, iy, r, g, b, a, layer);
			}
		}
	}
}

void draw_circle_with_alpha(int x, int y, int size_px, byte r, byte g, byte b, byte a, Art_layer* layer)
{
	float alpha = a/255.0;
	for(int ix=x-size_px; ix<=x+size_px; ix++)
	{
		for(int iy=y-size_px; iy<=y+size_px; iy++)
		{
			if((ix-x)*(ix-x) + (iy-y)*(iy-y) <= size_px*size_px)
			{
				rgba_t px = get_pixel(ix, iy, layer);
				byte new_r = px.r * (1.0-alpha) + r*alpha;
				byte new_g = px.g * (1.0-alpha) + g*alpha;
				byte new_b = px.b * (1.0-alpha) + b*alpha;
				draw_pixel(ix, iy, new_r, new_g, new_b, 255.0 - ((255-px.a)/255.0 * (255-a)/255.0)*255.0, layer);
			}
		}
	}
}

void erase_circle_with_alpha(int x, int y, int size_px, byte a, Art_layer* layer)
{
	x *= pr3_multiplier;
	y *= pr3_multiplier;
	size_px *= pr3_multiplier;
	float alpha = a/255.0;
	for(int ix=x-size_px; ix<=x+size_px; ix++)
	{
		for(int iy=y-size_px; iy<=y+size_px; iy++)
		{
			if((ix-x)*(ix-x) + (iy-y)*(iy-y) <= size_px*size_px)
			{
				rgba_t px = get_pixel(ix, iy, layer);
				draw_pixel(ix, iy, px.r, px.g, px.b, ceil(px.a * (1-alpha)), layer);
			}
		}
	}
}

void erase_circle(int x, int y, int size_px, byte a, Art_layer* layer)
{
	x *= pr3_multiplier;
	y *= pr3_multiplier;
	size_px *= pr3_multiplier;
	for(int ix=x-size_px; ix<=x+size_px; ix++)
	{
		for(int iy=y-size_px; iy<=y+size_px; iy++)
		{
			if((ix-x)*(ix-x) + (iy-y)*(iy-y) <= size_px*size_px)
			{
				rgba_t px = get_pixel(ix, iy, layer);
				draw_pixel(ix, iy, px.r, px.g, px.b, (a/255.0) * px.a, layer);
			}
		}
	}
}

void draw_circle_line(int x1, int y1, int x2, int y2, int size_px, byte r, byte g, byte b, byte a, Art_layer* layer)
{
	float ix = x1, iy = y1;
	float2 vec = {x2-x1, y2-y1};
	int dist = floor(sqrt(vec.x*vec.x + vec.y*vec.y));
	vec.x /= dist;
	vec.y /= dist;

	for(int i=0; i<dist; i++)
	{
		draw_circle(ix, iy, size_px, r, g, b, a, layer);
		ix += vec.x;
		iy += vec.y;
	}
}

void draw_circle_line_with_alpha(int x1, int y1, int x2, int y2, int size_px, byte r, byte g, byte b, byte a, Art_layer* layer)
{
	float ix = x1, iy = y1;
	float2 vec = {x2-x1, y2-y1};
	int dist = floor(sqrt(vec.x*vec.x + vec.y*vec.y));
	vec.x /= dist;
	vec.y /= dist;

	for(int i=0; i<dist; i++)
	{
		draw_circle_with_alpha(ix, iy, size_px, r, g, b, a, layer);
		ix += vec.x;
		iy += vec.y;
	}
}

void erase_circle_line_with_alpha(int x1, int y1, int x2, int y2, int size_px, byte a, Art_layer* layer)
{
	float ix = x1, iy = y1;
	float2 vec = {x2-x1, y2-y1};
	int dist = floor(sqrt(vec.x*vec.x + vec.y*vec.y));
	vec.x /= dist;
	vec.y /= dist;

	for(int i=0; i<dist; i++)
	{
		erase_circle_with_alpha(ix, iy, size_px, a, layer);
		ix += vec.x;
		iy += vec.y;
	}
}

void update_needing_art_chunks(Level* lvl)
{	
	for(int li=0; li<lvl->layer_list->elm_count; li++)
	{		
		Art_layer* layer = exl_get_elm_ptr(lvl->layer_list, li);
		for(int i=0; i<layer->art_chunk_list->elm_counter; i++)
		{			
			Art_chunk* chunk = ptrl_get_pointer(layer->art_chunk_list, i);
			if(chunk->needs_update)
			{
				gtex_update_texture(chunk->tex);
				chunk->needs_update = false;
			}
		}
	}	
}

byte hex_to_n(byte h)
{
	if(h >= '0' && h <= '9') return h - '0';
	return h - 'a' + 10;
}

byte3 convert_hex_to_color(char* hex)
{
	return (byte3)
	{
		hex_to_n(hex[0])*16 + hex_to_n(hex[1]),
		hex_to_n(hex[2])*16 + hex_to_n(hex[3]),
		hex_to_n(hex[4])*16 + hex_to_n(hex[5])
	};
}

void load_art_from_file(Level* lvl, FILE* file, int file_pos)
{
	char info_type;
	char color[7] = "000000";
	byte3 col;
	char separ;
	int size = 4;
	int pos_x, pos_y;
	byte alpha = 255;
	byte layer = 1;

	while(true)
	{		
		info_type = fgetc(file);
		if(info_type == EOF) break;
		if(info_type == 'c')
		{
			char t1, t2;
			t1 = fgetc(file);
			t2 = fgetc(file);
			if(t1 == '0' && t2 == ',')
			{
				col = (byte3) {0, 0, 0};
			}
			else
			{
				fseek(file, -2, SEEK_CUR);
				fgets(color, 7, file);

				if(color[4] == ',')
				{
					fseek(file, -2, SEEK_CUR);
					col = convert_hex_to_color(color);
					col.z = col.y;
					col.y = col.x;
					col.x = 0;
				}
				else if(color[2] == ',')
				{
					fseek(file, -4, SEEK_CUR);
					col = convert_hex_to_color(color);
					col.z = col.x;
					col.x = 0;
					col.y = 0;
				}
				else col = convert_hex_to_color(color);
				separ = fgetc(file);
			}
		}
		else if(info_type == 't')
		{
			fscanf(file, "%i", &size);
			separ = fgetc(file);
		}
		else if(info_type == 'm')
		{
			char letter = fgetc(file);
			if(letter == 'd')
			{
				alpha = 255;
				fseek(file, 3, SEEK_CUR);
			}
			if(letter == 'e')
			{
				alpha = 0;
				fseek(file, 4, SEEK_CUR);
			}
			separ = fgetc(file);
		}
		else if(info_type == 'd')
		{
			int x, y, result;
			pos_x = 0;
			pos_y = 0;

		repeat:;
			int prev_x = pos_x;
			int prev_y = pos_y;
			result = fscanf(file, "%i %c %i %c", &x, &separ, &y, &separ);
			pos_x += x;
			pos_y += y;

			int offset = 0;
			int offset_x = 0;
			if(layer == 2) offset = 12;
			else if(layer == 3) {offset = 17;}
			if(prev_x == 0)
			{				
				draw_circle(pos_x+offset_x, pos_y-offset, size/2, col.x, col.y, col.z, alpha, exl_get_elm_ptr(lvl->layer_list, layer));
			}
			else
			{
				draw_circle_line(prev_x+offset_x, prev_y-offset, pos_x, pos_y-offset, size/2, col.x, col.y, col.z, alpha, exl_get_elm_ptr(lvl->layer_list, layer));			
			}

		    if(result == EOF) break;
			if(separ == '`') { layer++; size = 4; alpha = 255;}
			if(layer == 4) {break;}
			if(separ == ';') goto repeat;
		}
		else
		{
			char debug[101];
			fread(debug, 1, 100, file);
			debug[100] = 0;
		}

	}
	update_needing_art_chunks(lvl);
}

void unlock_pixels(Art_layer* layer)
{
	for(int i=0; i<layer->art_chunk_list->elm_counter; i++)
	{
		Art_chunk* chunk = ptrl_get_pointer(layer->art_chunk_list, i);
		memset(chunk->px_locked, 0, ART_CHUNK_SZ*ART_CHUNK_SZ);
	}
}

