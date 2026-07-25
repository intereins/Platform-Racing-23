#include <math.h>
#include "items.h"
#include "player.h"
#include "sounds.h"
#include "block.h"
#include "level.h"
#include "system_funcs.h"
#include "tools.h"
#include "render.h"
#include "debug.h"
#include "animation.h"

#define MINE_PLACING_TIME 1000

Gl_texture* item_textures[ITEM_COUNT];
Gl_texture* tex_laser;
Gl_texture* tex_laser_light;
Extended_list* placed_mines_list = NULL;
//Extended_list* shot_lasers_list = NULL;

void init_items()
{
	placed_mines_list = exl_create(10, sizeof(Placed_mine), 2);
	//shot_lasers_list = exl_create(10, sizeof(Shot_laser), 2);

	item_textures[ITEM_TELEPORT] = gtex_load_texture("assets/img/items/teleport.png");
	item_textures[ITEM_SJUMP] = gtex_load_texture("assets/img/items/super_jump.png");
	item_textures[ITEM_SPEED] = gtex_load_texture("assets/img/items/speed_burst.png");
	item_textures[ITEM_GUN] = gtex_load_texture("assets/img/items/laser_gun.png");
	item_textures[ITEM_SWORD] = gtex_load_texture("assets/img/items/sword.png");
	item_textures[ITEM_JETPACK] = gtex_load_texture("assets/img/items/jet_pack.png");
	item_textures[ITEM_LIGHTNING] = gtex_load_texture("assets/img/items/lightning.png");
	item_textures[ITEM_ICEWAVE] = gtex_load_texture("assets/img/items/ice_wave.png");
	item_textures[ITEM_MINE] = gtex_load_texture("assets/img/items/mine.png");
	item_textures[ITEM_BLOCK] = gtex_load_texture("assets/img/items/block.png");

	tex_laser = gtex_load_texture("assets/img/items/bullet.png");
	tex_laser_light = gtex_load_texture("assets/img/items/bullet_light.png");
}

byte get_item_by_name(const char* name)
{
	if(strcmp(name, "sjump") == 0) return ITEM_SJUMP;
	if(strcmp(name, "speed") == 0) return ITEM_SPEED;
	if(strcmp(name, "teleport") == 0) return ITEM_TELEPORT;
	if(strcmp(name, "lightning") == 0) return ITEM_LIGHTNING;
	if(strcmp(name, "gun") == 0) return ITEM_GUN;
	if(strcmp(name, "sword") == 0) return ITEM_SWORD;
	if(strcmp(name, "mine") == 0) return ITEM_MINE;
	if(strcmp(name, "block") == 0) return ITEM_BLOCK;
	if(strcmp(name, "jetpack") == 0) return ITEM_JETPACK;
	if(strcmp(name, "icewave") == 0) return ITEM_ICEWAVE;

	return -1;
}

void give_item_to_player(Player* pl, enum Item item)
{
	if(player_has_hat(pl, HAT_ITEM))
	{
		pl->second_item = pl->held_item;
		pl->second_item_count = pl->item_count;
	}
    pl->held_item = item;
    if(item == ITEM_GUN || item == ITEM_SWORD || item == ITEM_ICEWAVE)
    {
        pl->item_count = 3;
    }
    else pl->item_count = 1;
}

void give_random_item_to_player(Player* pl, int allowed_items)
{
	int given_item;
	float divisor = 1;
	for(int i=0; i<ITEM_COUNT; i++)
	{
		if(allowed_items & 1)
		{
			if(random_float() <= 1 / divisor)
			{
				given_item = i;
			}
			divisor++;
		}
		allowed_items >>= 1;
	}
	give_item_to_player(pl, given_item);
}

void render_item(enum Item item, int slot)
{
	float offset = slot * 50;
	if(item == ITEM_MINE) {
		gtex_render_texture_wh(item_textures[ITEM_MINE], 10+offset, 10, 30, 30);
	}
	else if(item == ITEM_BLOCK) {
		gtex_render_texture_wh(item_textures[ITEM_BLOCK], 10+offset, 10, 30, 30);
	}
	else if(item == ITEM_GUN) {
		gtex_render_texture_wh(item_textures[ITEM_GUN], 8+offset, 10, 0.5*90, 0.5*57);
	}
	else if(item == ITEM_SWORD) {
		gtex_render_texture_wh(item_textures[ITEM_SWORD], 5+offset, 5, 18, 46);
	}
	else {
		gtex_render_texture_wh(item_textures[item], 5+offset, 5, 40, 40);
	}
}

void render_item_gui(Player* pl)
{
	if(pl->held_item >= 0 && pl->held_item <= ITEM_COUNT)
	{
		render_item(pl->held_item, 0);
	}
	if(player_has_hat(pl, HAT_ITEM))
	{
		render_item(pl->second_item, 1);
	}

	if(pl->held_item == ITEM_GUN)
	{
		//gtex_render_texture_wh(item_textures[ITEM_GUN], 271, 177, 90.0/4, 56.0/4);
		//render_texture_rot_point_scaled(item_textures[ITEM_GUN], 282, 184, rad(ang), 282-11, 184-5, 0.25);

	}
}

bool use_item_portable_block(Player* pl, int block_id)
{
	Placed_mine* mine = exl_add_get_ptr(placed_mines_list);
	mine->x = floor(pl->x/BLOCK_W);
	mine->y = floor((pl->y-5)/BLOCK_W);
	mine->block_id = block_id;
	mine->time_start = get_time_ms();
	play_sound(snd_mine);
}

FNCS_CREATE(gun_funcs, NULL, update_bullet, render_bullet);

Bullet* create_bullet(float x, float y, float vel_x, float vel_y)
{
	Bullet* ret = malloc(sizeof(Bullet));
	ret->object.fncs = &gun_funcs;
	ret->object.dead = false;
	ret->object.active = true;
	ret->x = x;
	ret->y = y;
	ret->vel_x = vel_x;
	ret->vel_y = vel_y;
	return ret;
}

void use_item(Player* pl)
{
    bool used = false;
    switch(pl->held_item)
    {
        case ITEM_SJUMP:
            if(pl->crawling) break;
            pl->vel_y -= 25;
			pl->y -= 7;
            play_sound(snd_sjump);
            used = true;
            break;

        case ITEM_TELEPORT:;
            int dest = pl->direction ? 120 : -120;
            if(get_block_type(pr2_get_block_from_pos(pl->x + dest, pl->y - 5, true)) == BA_INACTIVE)
            {
                pl->x += dest;
                play_sound(snd_teleport);
				start_anim(anmd_teleport, 193, 111, false, true);
                used = true;
            }
            break;

        case ITEM_SPEED:
            pl->speed_burst_timer = 5*27;
            pl->accel *= 2;
            pl->max_vel_x *= 2;
            play_sound(snd_speed_start);
            used = true;
            break;

        case ITEM_MINE:
            used = true;
            if(use_item_portable_block(pl, BL_MINE))
            {
                used = true;
            }
            break;

        case ITEM_BLOCK:
            used = true;
            if(use_item_portable_block(pl, BL_CRUMBLE))
            {
                used = true;
            }
            break;

		case ITEM_GUN:
			used = true;
			play_sound(snd_laser_shoot);
			add_obj(create_bullet(pl->direction ? pl->x : (pl->x-50), pl->y-30, pl->direction ? 12 : -12, 0));
			pl->vel_x += pl->direction ? -15 : 15;
			break;

		case ITEM_SWORD:
			used = true;
			play_sound(snd_sword);
			if(pl->direction)
			{
				start_anim(anmd_sword, 198, 121, true, true);
				pl->vel_x += 8;
				hit_block_with_sword(pr2_get_block(pl->x+30, pl->y-5, true, false));
				hit_block_with_sword(pr2_get_block(pl->x+30, pl->y-35, true, false));
				hit_block_with_sword(pr2_get_block(pl->x+60, pl->y-5, true, false));
				hit_block_with_sword(pr2_get_block(pl->x+60, pl->y-35, true, false));
			}
			else
			{
				start_anim(anmd_sword, 208, 121, false, true);
				pl->vel_x -= 8;
				hit_block_with_sword(pr2_get_block(pl->x-30, pl->y-5, true, false));
				hit_block_with_sword(pr2_get_block(pl->x-30, pl->y-35, true, false));
				hit_block_with_sword(pr2_get_block(pl->x-60, pl->y-5, true, false));
				hit_block_with_sword(pr2_get_block(pl->x-60, pl->y-35, true, false));
			}
			break;

        default:
            return;
    }

    if(used)
    {
        pl->item_count--;
        if(pl->item_count <= 0)
        {
			if(player_has_hat(pl, HAT_ITEM)) {
				pl->held_item = pl->second_item;
				pl->second_item = ITEM_NONE;
				pl->item_count = pl->second_item_count;
			} else {
				pl->held_item = ITEM_NONE;
			}
        }
    }
}

void render_placed_mines(float offset_x, float offset_y)
{
	time_t curr_time = get_time_ms();

	for(int i=0; i<placed_mines_list->elm_count; i++)
	{
		Placed_mine* mine = exl_get_elm_ptr(placed_mines_list, i);
		time_t time = curr_time - mine->time_start;

		float square_w = map_value_f(time, 0, MINE_PLACING_TIME, 90, 30) * 0.5;
		float square_h = square_w;
		float rect_alpha = sin((float)time/MINE_PLACING_TIME*3.14159265);

		glEnable(GL_TEXTURE_2D);
		gtex_render_texture(block_table[mine->block_id].texture, (mine->x+0.5)*BLOCK_W-square_w - offset_x, (mine->y+0.5)*BLOCK_H-square_h - offset_y, (mine->x+0.5)*BLOCK_W+square_w - offset_x, (mine->y+0.5)*BLOCK_H+square_h - offset_y);
		glDisable(GL_TEXTURE_2D);
		render_color_rect((mine->x+0.5)*BLOCK_W-square_w - offset_x, (mine->y+0.5)*BLOCK_H-square_h - offset_y, (mine->x+0.5)*BLOCK_W+square_w - offset_x, (mine->y+0.5)*BLOCK_H+square_h - offset_y, 1, 1, 1, rect_alpha);
	}
}

void handle_placed_mines()
{
	time_t time = get_time_ms();
	for(int i=0; i<placed_mines_list->elm_count; i++)
	{
		Placed_mine* mine = exl_get_elm_ptr(placed_mines_list, i);
		if(time - mine->time_start >= MINE_PLACING_TIME)
		{
			set_block(the_level, mine->x, mine->y, mine->block_id);
			exl_remove_elm(placed_mines_list, i);
			i--;
		}
	}
}

void update_bullet(Bullet* bullet)
{
	bullet->x += bullet->vel_x;
	bullet->y += bullet->vel_y;
	Block* block = pr2_get_block_from_pos(bullet->x, bullet->y, false);
	if(block != NULL)
	{
		switch(block_table[block->block_code].gun_reaction)
		{
			case IR_SHATTER:
				block_break(block);
				bullet->object.dead = true;
				break;

			case IR_EXPLODE:
				block_explode(block);
				bullet->object.dead = true;
				break;

			case IR_NONE:
				play_sound(snd_laser_hit);
				bullet->object.dead = true;
				break;


		}
	}
}

void render_bullet(Bullet* bullet)
{
	gtex_render_texture_def_size(tex_laser, bullet->x-g_cam_x, bullet->y-g_cam_y);
	gtex_render_texture_def_size(tex_laser_light, bullet->x-g_cam_x, bullet->y-9-g_cam_y);
}
