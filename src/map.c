#include "map.h"
#include "graph.h"
#include "def.h"
#include "player.h"

void render_level_map(Level* lvl, Player* pl)
{
	float img_ratio = (float)lvl->map->img->width / lvl->map->img->height;
	float x_size = 50/500.0 * img_ratio * 0.5 * DEF_WIN_W;
	float y_size = 50/400.0 * DEF_WIN_H;

	float map_x1 = DEF_WIN_W*0.5-x_size;
	float map_x2 = DEF_WIN_W*0.5+x_size;
	float map_y1 = 2;
	float map_y2 = 2+y_size;
	float map_w = map_x2-map_x1;
	float map_h = map_y2-map_y1;

	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);
	gtex_bind_texture(lvl->map);
	glBegin(GL_QUADS);

	glTexCoord2f(0, 1); glVertex2f(DEF_WIN_W*0.5 - x_size, 2 + y_size);
	glTexCoord2f(1, 1); glVertex2f(DEF_WIN_W*0.5 + x_size, 2 + y_size);
	glTexCoord2f(1, 0); glVertex2f(DEF_WIN_W*0.5 + x_size, 2);
	glTexCoord2f(0, 0); glVertex2f(DEF_WIN_W*0.5 - x_size, 2);

	glEnd();

	float2 pl_abs_pos = {pl->x/BLOCK_W, pl->y/BLOCK_H};

	float plx = (pl_abs_pos.x - lvl->map_border_x1) / (lvl->map_border_x2-lvl->map_border_x1) * map_w + map_x1;
	float ply = (pl_abs_pos.y - lvl->map_border_y1) / (lvl->map_border_y2-lvl->map_border_y1) * map_h + map_y1;


	glDisable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);

	float plw = 2;
	float plh = 2;

	glBegin(GL_QUADS);

	glColor3f(0, 0, 0);
	glVertex2f(plx-plw, ply+plh);
	glVertex2f(plx+plw, ply+plh);
	glVertex2f(plx+plw, ply-plh);
	glVertex2f(plx-plw, ply-plh);

	plw -= 1;
	plh -= 1;
	glColor3f(1, 1, 0);
	glVertex2f(plx-plw, ply+plh);
	glVertex2f(plx+plw, ply+plh);
	glVertex2f(plx+plw, ply-plh);
	glVertex2f(plx-plw, ply-plh);

	glEnd();
	glColor3f(1, 1, 1);

	glEnable(GL_TEXTURE_2D);
}

Gl_texture* create_level_map(Level* lvl)
{
	rgb_t block_avg_color = {0, 0, 0};
	Gl_texture* texture;

	//find 4 corners
	Chunk* first_chunk = exl_get_elm_ptr(lvl->chunk_list, 0);
	int lowest_x = first_chunk->x;
	int highest_x = first_chunk->x;
	int lowest_y = first_chunk->y;
	int highest_y = first_chunk->y;

	//low x
	for(int i=1; i<lvl->chunk_list->elm_count; i++)
	{
		Chunk* chunk = exl_get_elm_ptr(lvl->chunk_list, i);
		if(chunk->x < lowest_x)
		{
			lowest_x = chunk->x;
		}
	}
	//high x
	for(int i=1; i<lvl->chunk_list->elm_count; i++)
	{
		Chunk* chunk = exl_get_elm_ptr(lvl->chunk_list, i);
		if(chunk->x > highest_x)
		{
			highest_x = chunk->x;
		}
	}
	//low y
	for(int i=1; i<lvl->chunk_list->elm_count; i++)
	{
		Chunk* chunk = exl_get_elm_ptr(lvl->chunk_list, i);
		if(chunk->y < lowest_y)
		{
			lowest_y = chunk->y;
		}
	}
	//high y
	for(int i=1; i<lvl->chunk_list->elm_count; i++)
	{
		Chunk* chunk = exl_get_elm_ptr(lvl->chunk_list, i);
		if(chunk->y > highest_y)
		{
			highest_y = chunk->y;
		}
	}

	int size_x_ch = highest_x - lowest_x + 1;
	int size_y_ch = highest_y - lowest_y + 1;
	lvl->map_border_x1 = lowest_x*BLOCK_CHUNK_SZ;
	lvl->map_border_y1 = lowest_y*BLOCK_CHUNK_SZ;
	lvl->map_border_x2 = highest_x*BLOCK_CHUNK_SZ+BLOCK_CHUNK_SZ;
	lvl->map_border_y2 = highest_y*BLOCK_CHUNK_SZ+BLOCK_CHUNK_SZ;

	texture = gtex_create_texture_and_img(size_x_ch*BLOCK_CHUNK_SZ, size_y_ch*BLOCK_CHUNK_SZ, 32);

	for(int i=0; i<lvl->chunk_list->elm_count; i++)
	{
		Chunk* chunk = exl_get_elm_ptr(lvl->chunk_list, i);
		byte r=0, g=0, b=0, a=0;

		for(int y=0; y<BLOCK_CHUNK_SZ; y++)
		{
			for(int x=0; x<BLOCK_CHUNK_SZ; x++)
			{
				Block* block = get_block_from_chunk(chunk, x, y);

				if(block != NULL && block->block_code != 0)
				{
					r = block_table[block->block_code].avg_col.r;
					g = block_table[block->block_code].avg_col.g;
					b = block_table[block->block_code].avg_col.b;
					a = 255;
				}
				else
				{
					r = 0;
					g = 0;
					b = 0;
					a = 0;
				}

				iml_set_image_pixel_rgba(texture->img, (chunk->x-lowest_x)*BLOCK_CHUNK_SZ + x, (chunk->y-lowest_y)*BLOCK_CHUNK_SZ + y, r, g, b, a);
			}
		}
	}

	gtex_update_texture(texture);
	return texture;
}
















