#include <math.h>
#include "cJSON.h"
#include "block.h"
#include "gl_texture.h"
#include "graph.h"
#include "tools.h"
#include "player.h"
#include "level.h"
#include "block_piece.h"
#include "sounds.h"
#include "render.h"
#include "items.h"
#include "main.h"
#include "animation.h"

Block_info* block_table;
int block_count;

static bool any_side_does_action(Block_info *block, enum Block_action action);

int get_type_id(const char* type)
{
    if(strcmp(type, "active") == 0) return BT_ACTIVE;
    if(strcmp(type, "inactive") == 0) return BT_INACTIVE;
    if(strcmp(type, "water") == 0) return BT_WATER;
    if(strcmp(type, "impervious") == 0) return BT_IMPERVIOUS;
    if(strcmp(type, "net") == 0) return BT_NET;
    if(strcmp(type, "start") == 0) return BT_START;

    return -1;
}

int get_reaction_id(const char* reaction)
{
    if(strcmp(reaction, "pass") == 0) return IR_PASS;
    if(strcmp(reaction, "shatter") == 0) return IR_SHATTER;
    if(strcmp(reaction, "explode") == 0) return IR_EXPLODE;
    if(strcmp(reaction, "vanish") == 0) return IR_VANISH;
    if(strcmp(reaction, "push") == 0) return IR_PUSH;

    return -1;
}

int get_action_id(const char* action)
{
    if(strcmp(action, "active") == 0) return BA_ACTIVE;
    if(strcmp(action, "inactive") == 0) return BA_INACTIVE;
    if(strcmp(action, "ice") == 0) return BA_ICE;
    if(strcmp(action, "boost_0") == 0) return BA_BOOST_0;
    if(strcmp(action, "boost_90") == 0) return BA_BOOST_90;
    if(strcmp(action, "boost_180") == 0) return BA_BOOST_180;
    if(strcmp(action, "boost_270") == 0) return BA_BOOST_270;
    if(strcmp(action, "push") == 0) return BA_PUSH;
    if(strcmp(action, "vanish") == 0) return BA_VANISH;
    if(strcmp(action, "shatter") == 0) return BA_SHATTER;
    if(strcmp(action, "crumble") == 0) return BA_CRUMBLE;
    if(strcmp(action, "explode") == 0) return BA_EXPLODE;
    if(strcmp(action, "rot_90") == 0) return BA_ROT_90;
    if(strcmp(action, "rot_180") == 0) return BA_ROT_180;
    if(strcmp(action, "rot_270") == 0) return BA_ROT_270;
    if(strcmp(action, "stat_inc") == 0) return BA_STAT_INC;
    if(strcmp(action, "stat_dec") == 0) return BA_STAT_DEC;
    if(strcmp(action, "item") == 0) return BA_ITEM;
    if(strcmp(action, "bounce") == 0) return BA_BOUNCE;
    if(strcmp(action, "teleport") == 0) return BA_TELEPORT;

    return -1;
}



void load_blocks(const char* filename)
{
    char* contents = load_file(filename);
    cJSON* json = cJSON_Parse(contents);
    block_count = cJSON_GetArraySize(json) + 1;
    block_table = calloc(block_count, sizeof(Block_info));
    cJSON* block = NULL;
    int i = 1;

	cJSON_ArrayForEach(block, json)
    {
        Block_info* block_info = &block_table[i];
        cJSON* id = cJSON_GetObjectItem(block, "id");
		cJSON* name = cJSON_GetObjectItem(block, "name");
		cJSON* texture = cJSON_GetObjectItem(block, "texture");
		cJSON* type = cJSON_GetObjectItem(block, "type");
		cJSON* up = cJSON_GetObjectItem(block, "up");
		cJSON* down = cJSON_GetObjectItem(block, "down");
		cJSON* left = cJSON_GetObjectItem(block, "left");
		cJSON* right = cJSON_GetObjectItem(block, "right");
		cJSON* bump = cJSON_GetObjectItem(block, "bump");
		cJSON* safe = cJSON_GetObjectItem(block, "safe");
		cJSON* bump_anim = cJSON_GetObjectItem(block, "bump_anim");
		//cJSON* boost_power = cJSON_GetObjectItem(block, "boost_power");
		cJSON* gun_reaction = cJSON_GetObjectItem(block, "gun");
		cJSON* sword_reaction = cJSON_GetObjectItem(block, "sword");
		cJSON* move_pattern = cJSON_GetObjectItem(block, "move_pattern");
		cJSON* items = cJSON_GetObjectItem(block, "items");

        Gl_texture* tex = gtex_load_texture(texture->valuestring);
        block_info->avg_col = get_image_avg_color(tex->img);
        block_info->texture = tex == NULL ? tex_block_not_found : tex;
        block_info->type = get_type_id(type->valuestring);
        block_info->action_top = up == NULL ? BA_ACTIVE : get_action_id(up->valuestring);
        block_info->action_bottom = down == NULL ? BA_ACTIVE : get_action_id(down->valuestring);
        block_info->action_left = left == NULL ? BA_ACTIVE : get_action_id(left->valuestring);
        block_info->action_right = right == NULL ? BA_ACTIVE : get_action_id(right->valuestring);
        block_info->action_bump = bump == NULL ? BA_ACTIVE : get_action_id(bump->valuestring);
        block_info->sword_reaction = sword_reaction == NULL ? IR_NONE : get_reaction_id(sword_reaction->valuestring);
        block_info->gun_reaction = gun_reaction == NULL ? IR_NONE : get_reaction_id(gun_reaction->valuestring);

        if(safe == NULL)
        {
            block_info->safe = true;

            if(any_side_does_action(block_info, BA_SHATTER)) block_info->safe = false;
            else if(any_side_does_action(block_info, BA_CRUMBLE)) block_info->safe = false;
            else if(any_side_does_action(block_info, BA_PUSH)) block_info->safe = false;
            else if(any_side_does_action(block_info, BA_VANISH)) block_info->safe = false;

            if(move_pattern != NULL) block_info->safe = false;
            if(block_info->action_top == BA_INACTIVE) block_info->safe = false;

        }

        if(items != NULL)
        {
            cJSON* item = NULL;
            cJSON_ArrayForEach(item, items)
            {
                byte item_id = get_item_by_name(item->valuestring);
                block_table[i].allowed_items |= 1 << item_id;
            }
        }

        if(move_pattern != NULL)
        {
            int pattern_len = cJSON_GetArraySize(move_pattern);
            block_table[i].can_move = true;
            block_table[i].move_pattern = malloc(pattern_len * 2);
            block_table[i].move_pattern_len = pattern_len;
            cJSON* delay = cJSON_GetObjectItem(block, "move_delay");
            if(delay != NULL)
            {
                block_table[i].move_delay = delay->valueint;
            }
            cJSON* move = NULL;
            int j=0;
            cJSON_ArrayForEach(move, move_pattern)
            {
                int amount;
                char dir[8];
                sscanf(move->valuestring, "%7s %i", dir, &amount);
                if(strcmp(dir, "up") == 0) block_table[i].move_pattern[j] = MOVE_UP;
                else if(strcmp(dir, "down") == 0) block_table[i].move_pattern[j] = MOVE_DOWN;
                else if(strcmp(dir, "left") == 0) block_table[i].move_pattern[j] = MOVE_LEFT;
                else if(strcmp(dir, "right") == 0) block_table[i].move_pattern[j] = MOVE_RIGHT;
                else if(strcmp(dir, "wait") == 0) block_table[i].move_pattern[j] = MOVE_WAIT;
                else if(strcmp(dir, "random") == 0) block_table[i].move_pattern[j] = MOVE_RANDOM;
                else if(strcmp(dir, "return") == 0) block_table[i].move_pattern[j] = MOVE_RETURN;

                block_table[i].move_pattern[j+1] = (byte)amount;

                printf("MOVE %i %i\n", block_table[i].move_pattern[j], block_table[i].move_pattern[j+1]);
                j+=2;
            }

        }

        i++;
    }
}

static bool any_side_does_action(Block_info* block, enum Block_action action)
{
    if(block->action_top == action) return true;
    if(block->action_bottom == action) return true;
    if(block->action_right == action) return true;
    if(block->action_left == action) return true;
    if(block->action_bump == action) return true;

    return false;
}

enum Block_type get_block_type(Block* block)
{
    if(block == NULL) return BA_INACTIVE;

    return block_table[block->block_code].type;
}

int convert_pr2old_id_to_pr4(int id)
{
    switch(id)
    {
        case  0: return BL_BASIC_1;
        case  1: return BL_BASIC_2;
        case  2: return BL_BASIC_3;
        case  3: return BL_BASIC_4;
        case  4: return BL_BRICKS;
        case  5: return BL_ARROW_DOWN;
        case  6: return BL_ARROW_UP;
        case  7: return BL_ARROW_LEFT;
        case  8: return BL_ARROW_RIGHT;
        case  9: return BL_MINE;
        case 10: return BL_ITEM;
        case 11: return 2;
        case 12: return 2;
        case 13: return 2;
        case 14: return 2;
        case 15: return BL_ICE;
        case 16: return BL_FINISH;
        case 17: return BL_CRUMBLE;
        case 18: return BL_VANISH;
        case 19: return BL_MOVE;
        case 20: return BL_WATER;
        case 21: return BL_ROT_R;
        case 22: return BL_ROT_L;
        case 23: return BL_PUSH;
        case 24: return BL_NET;
        case 25: return BL_ITEM_INF;
        case 26: return BL_STAT_INC;
        case 27: return BL_STAT_DEC;
    }
    return 3;
}

Gl_texture* get_block_texture_by_id(int id)
{
    return block_table[id].texture;
}

void block_render(Block* block, number_t x, number_t y, float rot)
{
    Gl_texture* tex = get_block_texture_by_id(block->block_code);
    if(tex != NULL)
    {
        float col = 1;
        if(block->used || block->teleport_timer > 0)
        {
            col = 0.5;
        }
        if(block->color != 0)
        {
            render_color_rect_whf(x, y+block->offset_y*30, BLOCK_W, BLOCK_H, (block->color >> 16)/255.0*col, ((block->color >> 8) & 255)/255.0*col, (block->color & 255)/255.0*col);
        }

        if(block->can_vanish)
        {
            float alpha = 1.0 - (float)block->vanish_timer / block->vanish_len1;
            glColor4f(col, col, col, alpha);
            render_texture_rot_center(tex, x, y+block->offset_y*30, x+BLOCK_W, y+block->offset_y*30+BLOCK_H, rot, current_rotation);
            glColor4f(col, col, col, 1);
        }
        else
        {
            glColor3f(col, col, col);
            render_texture_rot_center(tex, x, y+block->offset_y*30, x+BLOCK_W, y+block->offset_y*30+BLOCK_H, rot, current_rotation);
        }

        if(block->frozen)
        {
            glColor4f(1, 1, 1, (block->freeze_timer - 20.0) / 80.0);
            render_texture_rot_center(tex_block_frozen, x, y+block->offset_y*30, x+BLOCK_W, y+block->offset_y*30+BLOCK_H, rot, current_rotation);
        }
        glColor4f(1, 1, 1, 1);
    }
}

bool block_is_active(Block* block, Player* pl)
{
    if(block->frozen) return true;

    if(block->can_vanish)
    {
        if(player_has_hat(pl, HAT_TOP)) return false;

        return block->vanish_timer < block->vanish_len1;
    }
    return block->block_code != BL_NET && block->block_code != BL_WATER;
}

double2 block_method_18(Block* block, number_t rotation)
{
    //TODO
    return (double2) {.x = get_block_pos_x(block), .y = get_block_pos_y(block)};
}

void block_method_315(Block* block, number_t param1, number_t param2)
{
    double2 _loc3_ = data_method_9(param1,param2,0);
    block_hit(block, _loc3_.x,_loc3_.y);
}

bool block_is_safe(Block* block)
{
    return block_table[block->block_code].safe;
}

bool block_is_fragile(int id)
{
    return get_block_action_from_id(id, SIDE_TOP) == BA_SHATTER
    || get_block_action_from_id(id, SIDE_RIGHT) == BA_SHATTER
    || get_block_action_from_id(id, SIDE_BOTTOM) == BA_SHATTER
    || get_block_action_from_id(id, SIDE_LEFT) == BA_SHATTER;
}

void block_damage(Block* block, float power)
{
    printf("damage %f\n", power);
    int val = floor(power) / 4;
    block->life -= val;
    block_throw_pieces(block, val*2);
    if(block->life <= 0)
    {
        block_break(block);
    }
}

bool block_can_freeze(Block* block)
{
    return true;
}

void block_freeze(Block* block, bool param1)
{
    if(block->freeze_timer == 0)
    {
        block->frozen = true;
        block->freeze_timer = 100;
    }
    /*if(!block->frozen)
    {
        block->frozen = true;
        block->var_600 = Data.getTimestamp();
        block->var_110 = new Bitmap(Blocks.iceBitmap);
        addChild(block->var_110);
        if(param1)
        {
            block->var_110.alpha = 1.75;
            block->var_455 = 0.01;
        }
        else
        {
            block->var_110.alpha = 1;
            block->var_455 = 0.025;
        }
    }*/
}

void block_on_stand(Block* block, Player* pl)
{
    double2 _loc2_;
    number_t prev_vel = pl->vel_y;
    if(!block->frozen /*&& block->method_777() > 4*/ && player_has_hat(pl, HAT_SANTA) && block_can_freeze(block))
    {
        block_freeze(block, false);
    }
    if(block->frozen)
    {
        pl->var_147 = 0.05;
    }
    //if(block_is_active(block, pl))
    {
        //TODO nan
        _loc2_ = block_method_18(block, 0);
        pl->y = _loc2_.y + get_block_pos_y(block) - get_block_pos_y(block); //TODO what last
        pl->vel_y = 0;
        pl->on_ground = true;
        //TODO safe stand
        if(block_is_safe(block))
        {
            pl->safe_x = _loc2_.x + 15;
            pl->safe_y = _loc2_.y;
            pl->var_407 = get_block_pos_x(block) / BLOCK_W;
            pl->var_366 = get_block_pos_y(block) / BLOCK_H;
        }
    }
    //else
    {
        //pl->on_ground = false;
    }

    switch(get_block_action(pl, block, SIDE_TOP, current_rotation))
    {
        case BA_ICE:
            pl->var_147 = 0.05; break;
        case BA_BOOST_0:
        case BA_BOOST_90:
        case BA_BOOST_180:
        case BA_BOOST_270:
            block_on_stand_boost(block, pl); break;

        case BA_PUSH:
            push_block(the_level, DIR_DOWN, get_block_x(block), get_block_y(block)); break;

        case BA_VANISH:
            block_vanish(block); break;

        case BA_CRUMBLE:
            if(pl->down && player_has_hat(pl, HAT_CHEESE))
            {
                block_damage(block, 50); break;
            }
            block_damage(block, prev_vel*1.75); break;

        case BA_EXPLODE:
            block_on_touch_mine(block, pl); break;

        case BA_BOUNCE:
            pl->vel_y = -prev_vel;
            if(prev_vel > 0.8)
            {
                play_sound(snd_bounce);
            }
            break;

        case BA_TELEPORT:
            use_teleport_block(block, pl);
            break;
    }
}

void block_on_left_hit(Block* block, Player* pl)
{
    double2 _loc2_;
    number_t prev_vel = pl->vel_x;

    if(get_block_action(pl, block, SIDE_LEFT, current_rotation) == BA_CRUMBLE && player_has_hat(pl, HAT_CHEESE))
    {

    }
    else if(block_is_active(block, pl))
    {
        _loc2_ = block_method_18(block, 0);
        pl->x = _loc2_.x - pl->var_189;

        if(pl->vel_x > 0)
        {
            pl->vel_x *= -0.05;
        }
        if(pl->var_24 > 0)
        {
            pl->var_24 = 0;
        }
    }

    switch(get_block_action(pl, block, SIDE_LEFT, current_rotation))
    {
        case BA_BOOST_0:
        case BA_BOOST_90:
        case BA_BOOST_180:
        case BA_BOOST_270:
            block_on_left_hit_boost(block, pl); break;

        case BA_PUSH:
            push_block(the_level, DIR_RIGHT, get_block_x(block), get_block_y(block)); break;

        case BA_VANISH:
            if(!player_has_hat(pl, HAT_TOP)) block_vanish(block); break;

        case BA_CRUMBLE:
            block_damage(block, player_has_hat(pl, HAT_CHEESE) ? 50 : prev_vel*1.75); break;

        case BA_EXPLODE:
            block_on_touch_mine(block, pl); break;

        case BA_BOUNCE:
            pl->vel_x = -prev_vel;
            if(prev_vel > 0.8)
            {
                play_sound(snd_bounce);
            }
            break;

        case BA_SHATTER:
            block_break(block); break;

        case BA_ROT_90:
            rotate_player(pl, ROT_90); break;

        case BA_ROT_270:
            rotate_player(pl, ROT_270); break;

        case BA_STAT_INC:
            if(block->used) break;
            player_set_stats(pl, pl->speed_stat+5, pl->accel_stat+5, pl->jumpn_stat+5, true);
            play_sound(snd_happy);
            block->used = true;
            break;

        case BA_STAT_DEC:
            if(block->used) break;
            player_set_stats(pl, pl->speed_stat-5, pl->accel_stat-5, pl->jumpn_stat-5, true);
            play_sound(snd_sad);
            block->used = true;
            break;

        case BA_ITEM:
            if(block->used) break;
            play_sound(snd_get_item);
            give_random_item_to_player(pl, block_table[block->block_code].allowed_items);
            if(block->item_count != 255)
            {
                block->item_count--;
                if(block->item_count == 0)
                {
                    block->used = true;
                }
            }
            break;

        case BA_TELEPORT:
            use_teleport_block(block, pl);
            break;
    }
}

void block_on_right_hit(Block* block, Player* pl)
{
    number_t prev_vel = pl->vel_x;
    double2 _loc2_;

    if(get_block_action(pl, block, SIDE_RIGHT, current_rotation) == BA_CRUMBLE && player_has_hat(pl, HAT_CHEESE))
    {

    }
    else if(block_is_active(block, pl))
    {
        _loc2_ = block_method_18(block, 0);
        pl->x = _loc2_.x + block->size + pl->var_189;
        if(pl->vel_x < 0)
        {
            pl->vel_x *= -0.05;
        }
        if(pl->var_24 < 0)
        {
            pl->var_24 = 0;
        }
    }

    switch(get_block_action(pl, block, SIDE_RIGHT, current_rotation))
    {
        case BA_BOOST_0:
        case BA_BOOST_90:
        case BA_BOOST_180:
        case BA_BOOST_270:
            block_on_right_hit_boost(block, pl); break;

        case BA_PUSH:
            push_block(the_level, DIR_LEFT, get_block_x(block), get_block_y(block)); break;

        case BA_VANISH:
            if(!player_has_hat(pl, HAT_TOP)) block_vanish(block); break;

        case BA_CRUMBLE:
            block_damage(block, player_has_hat(pl, HAT_CHEESE) ? 50 : -prev_vel*1.75); break;

        case BA_EXPLODE:
            block_on_touch_mine(block, pl); break;

        case BA_BOUNCE:
            pl->vel_x = -prev_vel;
            if(prev_vel > 0.8)
            {
                play_sound(snd_bounce);
            }
            break;

        case BA_SHATTER:
            block_break(block); break;

        case BA_ROT_90:
            rotate_player(pl, ROT_90); break;

        case BA_ROT_270:
            rotate_player(pl, ROT_270); break;

        case BA_STAT_INC:
            if(block->used) break;
            player_set_stats(pl, pl->speed_stat+5, pl->accel_stat+5, pl->jumpn_stat+5, true);
            play_sound(snd_happy);
            block->used = true;
            break;

        case BA_STAT_DEC:
            if(block->used) break;
            player_set_stats(pl, pl->speed_stat-5, pl->accel_stat-5, pl->jumpn_stat-5, true);
            play_sound(snd_sad);
            block->used = true;
            break;

        case BA_ITEM:
            if(block->used) break;
            play_sound(snd_get_item);
            give_random_item_to_player(pl, block_table[block->block_code].allowed_items);
            if(block->item_count != 255)
            {
                block->item_count--;
                if(block->item_count == 0)
                {
                    block->used = true;
                }
            }
            break;

        case BA_TELEPORT:
            use_teleport_block(block, pl);
            break;
    }
}

void block_on_touch(Block* block, Player* pl)
{
    if(block->block_code == BL_NET)
    {
        block_on_touch_net(block, pl);
    }
    else if(block->block_code == BL_WATER)
    {
        block_on_touch_water(block, pl);
    }
    else if(block->block_code == BL_MINE)
    {
        block_on_touch_mine(block, pl);
    }
}

void block_on_touch_net(Block* block, Player* pl)
{
    double2 _loc2_;
    if(!block->frozen)
    {
        _loc2_ = (double2) {get_block_pos_x(block)/BLOCK_W, get_block_pos_y(block)/BLOCK_H};
        if(pl->var_407 != _loc2_.x || pl->var_366 < _loc2_.y || pl->var_366 > _loc2_.y + 2)
        {
            player_return_to_safe(pl);
        }
    }
}

void block_on_touch_water(Block* block, Player* pl)
{
    double2 _loc2_;
    double2 _loc3_;
    if(!block->frozen)
    {
        if(!pl->on_ground && pl->mode != PLAYER_MODE_FREEZE && pl->mode != PLAYER_MODE_HURT)
        {
            player_set_mode(pl, PLAYER_MODE_WATER);
            pl->var_240 = 2;
        }
        else
        {
            pl->var_24 *= 0.9;
            pl->var_147 = 0.1;
        }

        _loc2_ = block_method_18(block, current_rotation);
        _loc3_ = (double2) {.x = get_block_x(block), .y = get_block_y(block)};
        pl->var_407 = _loc3_.x;
        pl->var_366 = _loc3_.y;
        pl->safe_x = _loc2_.x + 15;
        pl->safe_y = _loc2_.y + 15;
        //TODO block_method_339(block);
    }
}

void block_on_bump(Block* block, Player* pl)
{
    number_t prev_vel = pl->vel_y;
    double2 _loc2_;
    double2 _loc3_;

    byte action = get_block_action(pl, block, SIDE_BUMP, current_rotation);
    if((action == BA_CRUMBLE || action == BA_SHATTER) && player_has_hat(pl, HAT_CHEESE))
    {

    }
    else if(block_is_active(block, pl))
    {
        _loc2_ = block_method_18(block, 0);
        _loc3_ = data_method_9(/*TODO what? get_block_x(block) - get_block_pos_x(block)*/0, /*get_block_y(block) - get_block_pos_y(block)*/0, 0);
        if(pl->crawling)
        {
            pl->y = _loc2_.y + block->size + _loc3_.y + pl->var_325 / 2.0;
        }
        else
        {
            pl->y = _loc2_.y + block->size + _loc3_.y + pl->var_325;
        }
        pl->vel_y *= -0.25;
        pl->const_12 = 0;
        if(block->var_490)
        {
            block_method_315(block, 0,-15);
        }
        if(get_block_action(pl, block, SIDE_BOTTOM, pl->rot) < BA_BOOST_0 || get_block_action(pl, block, SIDE_BOTTOM, pl->rot) > BA_BOOST_270)
        {
            block->vel_y = -1.0/30;
            play_bump_sound(get_block_x(block), get_block_y(block), 1);
        }
    }

    enum Block_side side = SIDE_BOTTOM;
repeat_for_bump:
    switch(get_block_action(pl, block, side, current_rotation))
    {
        case BA_BOOST_0:
            block_on_bump_boost(block, pl, BA_BOOST_0); break;
        case BA_BOOST_90:
            block_on_bump_boost(block, pl, BA_BOOST_90); break;
        case BA_BOOST_180:
            block_on_bump_boost(block, pl, BA_BOOST_180); break;
        case BA_BOOST_270:
            block_on_bump_boost(block, pl, BA_BOOST_270); break;

        case BA_PUSH:
            push_block(the_level, DIR_UP, get_block_x(block), get_block_y(block)); break;

        case BA_VANISH:
            block_vanish(block); break;

        case BA_SHATTER:
            block_break(block); break;

        case BA_CRUMBLE:
            block_damage(block, player_has_hat(pl, HAT_CHEESE) ? 50 : -prev_vel*1.75); break;

        case BA_EXPLODE:
            block_on_touch_mine(block, pl); break;

        case BA_ROT_90:
            rotate_player(pl, ROT_90); break;

        case BA_ROT_270:
            rotate_player(pl, ROT_270); break;

        case BA_STAT_INC:
            if(block->used) break;
            player_set_stats(pl, pl->speed_stat+5, pl->accel_stat+5, pl->jumpn_stat+5, true);
            play_sound(snd_happy);
            block->used = true;
            break;

        case BA_STAT_DEC:
            if(block->used) break;
            player_set_stats(pl, pl->speed_stat-5, pl->accel_stat-5, pl->jumpn_stat-5, true);
            play_sound(snd_sad);
            block->used = true;
            break;

        case BA_ITEM:
            if(block->used) break;
            play_sound(snd_get_item);
            give_random_item_to_player(pl, block_table[block->block_code].allowed_items);
            if(block->item_count != 255)
            {
                block->item_count--;
                if(block->item_count == 0)
                {
                    block->used = true;
                }
            }
            break;

        case BA_BOUNCE:
            pl->vel_y = -prev_vel;
            if(prev_vel > 0.8)
            {
                play_sound(snd_bounce);
            }
            break;

        case BA_TELEPORT:
            use_teleport_block(block, pl);
            break;

    }

    if(side == SIDE_BOTTOM)
    {
        side = SIDE_BUMP;
        goto repeat_for_bump;
    }
}

void block_hit(Block* block, number_t x, number_t y)
{

}

number_t block_boost_get_dir(Block* block)
{
    return 90;
}

void block_boost_push(Player* pl, number_t dir)
{
    if(dir == 0 && pl->crawling == false)
    {
        pl->vel_y -= 1.2;
    }
    if(dir == 180 || dir == -180)
    {
        pl->vel_y += 5;
    }
    if(dir == -90)
    {
        pl->vel_x -= 3;
    }
    if(dir == 90)
    {
        pl->vel_x += 3;
    }
}

void block_on_stand_boost(Block* block, Player* pl)
{
    //number_t _loc2_ = block_boost_get_dir(block);
    if(get_block_action(pl, block, SIDE_TOP, current_rotation) == BA_BOOST_0 && !pl->crawling)
    {
        pl->vel_y -= 10;
    }
    else
    {
        switch(get_block_action(pl, block, SIDE_TOP, current_rotation))
        {
            case BA_BOOST_90:  block_boost_push(pl, 90); break;
            case BA_BOOST_180: block_boost_push(pl, 180); break;
            case BA_BOOST_270: block_boost_push(pl, -90); break;
        }
        //block_boost_push(pl, _loc2_);
    }
    //TODO display arrow
    //this.method_87();
}

void block_on_bump_boost(Block* block, Player* pl, int boost_dir)
{
    if(boost_dir == BA_BOOST_180)
    {
        pl->vel_y = pl->down == false && pl->crawling == false ? -14 : 0;
    }
    else
    {
        switch(boost_dir)
        {
            case BA_BOOST_90:  block_boost_push(pl, -90); break;
            case BA_BOOST_180: block_boost_push(pl, 180); break;
            case BA_BOOST_270: block_boost_push(pl, 90); break;
        }
    }
}

void block_on_left_hit_boost(Block* block, Player* pl)
{
    switch(get_block_action(pl, block, SIDE_LEFT, current_rotation))
    {
        case BA_BOOST_0:   block_boost_push(pl, -90); break;
        case BA_BOOST_90:  block_boost_push(pl, 0); break;
        case BA_BOOST_180: block_boost_push(pl, 90); break;
        case BA_BOOST_270: block_boost_push(pl, 180); break;
    }
}

void block_on_right_hit_boost(Block* block, Player* pl)
{
    switch(get_block_action(pl, block, SIDE_RIGHT, current_rotation))
    {
        case BA_BOOST_0:   block_boost_push(pl, 90); break;
        case BA_BOOST_90:  block_boost_push(pl, 180); break;
        case BA_BOOST_180: block_boost_push(pl, -90); break;
        case BA_BOOST_270: block_boost_push(pl, 0); break;
    }
}

byte get_block_action(Player* pl, Block* block, enum Block_side side, int lvl_rot)
{
    if(block == NULL)
    {
        return BA_INACTIVE;
    }

    byte action;
    if(side == SIDE_BUMP)
    {
        action = block_table[block->block_code].action_bump;
    }
    else switch((side + (4-lvl_rot)) % 4)
    {
        case SIDE_TOP: action = block_table[block->block_code].action_top; break;
        case SIDE_BOTTOM: action = block_table[block->block_code].action_bottom; break;
        case SIDE_LEFT: action = block_table[block->block_code].action_left; break;
        case SIDE_RIGHT: action = block_table[block->block_code].action_right; break;

    }

    if(action == BA_INACTIVE && block->frozen && side == SIDE_TOP)
    {
        return BA_ICE;
    }
    /*switch(side)
    {
        case SIDE_TOP: action = block->action_top; break;
        case SIDE_BOTTOM: action = block->action_bottom; break;
        case SIDE_LEFT: action = block->action_left; break;
        case SIDE_RIGHT: action = block->action_right; break;
    }*/

    if(action == BA_VANISH && player_has_hat(pl, HAT_TOP))
    {
        if(!(side == SIDE_TOP /*&& pl->y <= block->pos_y+15*/))
            return BA_INACTIVE;
    }
    return action;
}

byte get_block_action_from_id(int id, int side)
{
    if(id == BL_BASIC_1) return BA_ACTIVE;
    if(id == BL_BASIC_2) return BA_ACTIVE;
    if(id == BL_BASIC_3) return BA_ACTIVE;
    if(id == BL_BASIC_4) return BA_ACTIVE;
    if(id == BL_MOVE) return BA_ACTIVE;

    if(id == BL_ICE) return BA_ICE;
    if(id == BL_PUSH) return BA_PUSH;
    if(id == BL_VANISH) return BA_VANISH;
    if(id == BL_CRUMBLE) return BA_CRUMBLE;
    if(id == BL_MINE) return BA_EXPLODE;
    if(id == BL_WATER) return BA_INACTIVE;
    if(id == BL_NET) return BA_INACTIVE;
    if(id == BL_ROT_R) return BA_ROT_90;
    if(id == BL_ROT_L) return BA_ROT_270;
    if(id == BL_BOUNCE) return BA_BOUNCE;

    if(id == BL_PUSH_HOR)
    {
        if(side == SIDE_LEFT || side == SIDE_RIGHT) return BA_PUSH;
        return BA_ACTIVE;
    }

    if(id == BL_BRICKS)
    {
        if(side == SIDE_BOTTOM) return BA_SHATTER;
        else return BA_ACTIVE;
    }
    if(id == BL_STAT_INC)
    {
        if(side == SIDE_BOTTOM) return BA_STAT_INC;
        else return BA_ACTIVE;
    }
    if(id == BL_STAT_DEC)
    {
        if(side == SIDE_BOTTOM) return BA_STAT_DEC;
        else return BA_ACTIVE;
    }
    if(id == BL_ITEM || id == BL_ITEM_INF)
    {
        if(side == SIDE_BOTTOM) return BA_ITEM;
        else return BA_ACTIVE;
    }

    if(id == BL_ARROW_UP)
    {
        switch(side)
        {
            case SIDE_TOP:    return BA_BOOST_0;
            case SIDE_RIGHT:  return BA_BOOST_270;
            case SIDE_BOTTOM: return BA_BOOST_180;
            case SIDE_LEFT:   return BA_BOOST_90;
        }
    }
    else if(id == BL_ARROW_RIGHT)
    {
        switch(side)
        {
            case SIDE_TOP:    return BA_BOOST_90;
            case SIDE_RIGHT:  return BA_BOOST_0;
            case SIDE_BOTTOM: return BA_BOOST_270;
            case SIDE_LEFT:   return BA_BOOST_180;
        }
    }
    else if(id == BL_ARROW_DOWN)
    {
        switch(side)
        {
            case SIDE_TOP:    return BA_BOOST_180;
            case SIDE_RIGHT:  return BA_BOOST_90;
            case SIDE_BOTTOM: return BA_BOOST_0;
            case SIDE_LEFT:   return BA_BOOST_270;
        }
    }
    else if(id == BL_ARROW_LEFT)
    {
        switch(side)
        {
            case SIDE_TOP:    return BA_BOOST_270;
            case SIDE_RIGHT:  return BA_BOOST_180;
            case SIDE_BOTTOM: return BA_BOOST_90;
            case SIDE_LEFT:   return BA_BOOST_90;
        }
    }

    if(id == BL_1WAY_UP)
    {
        switch(side)
        {
            case SIDE_TOP:    return BA_ACTIVE;
            case SIDE_RIGHT:  return BA_ACTIVE;
            case SIDE_BOTTOM: return BA_INACTIVE;
            case SIDE_LEFT:   return BA_ACTIVE;
        }
    }
    else if(id == BL_1WAY_RIGHT)
    {
        switch(side)
        {
            case SIDE_TOP:    return BA_INACTIVE;
            case SIDE_RIGHT:  return BA_ACTIVE;
            case SIDE_BOTTOM: return BA_INACTIVE;
            case SIDE_LEFT:   return BA_INACTIVE;
        }
    }
    else if(id == BL_1WAY_DOWN)
    {
        switch(side)
        {
            case SIDE_TOP:    return BA_INACTIVE;
            case SIDE_RIGHT:  return BA_ACTIVE;
            case SIDE_BOTTOM: return BA_ACTIVE;
            case SIDE_LEFT:   return BA_ACTIVE;
        }
    }
    else if(id == BL_1WAY_LEFT)
    {
        switch(side)
        {
            case SIDE_TOP:    return BA_INACTIVE;
            case SIDE_RIGHT:  return BA_INACTIVE;
            case SIDE_BOTTOM: return BA_INACTIVE;
            case SIDE_LEFT:   return BA_ACTIVE;
        }
    }
}

void update_block_offset(Block* block)
{
    if(block->offset_y <= 0 && block->vel_y != 0)
    {
        block->offset_y += block->vel_y;
        block->vel_y += 0.1/30;
        block->vel_y *= 0.95;
        if(block->offset_y > 0)
        {
            block->offset_y = 0;
        }
        else if(block->offset_y < -0.4)
        {
            block->offset_y = -0.4;
            block->vel_y = 0.001;
        }
    }
}

void block_update(Block* block)
{
    if(block == NULL) return;
    if(block->last_update >= frame_nr) return;

    block->last_update = frame_nr;

    if(block_table[block->block_code].can_move)
    {
        block->move_timer--;
        if(block->move_timer <= 0)
        {
            byte* pattern = block_table[block->block_code].move_pattern;
            if(pattern == NULL) return;
            block->move_timer = block_table[block->block_code].move_delay;
            int2 rot_pos = rotate_position(block->x, block->y, current_rotation, 1999);

            if(pattern[block->move_counter] == MOVE_RANDOM)
            {
                push_block(the_level, rand()%4, rot_pos.x, rot_pos.y);
            }
            else if(pattern[block->move_counter] == MOVE_RETURN)
            {
                int2 ret_pos = rotate_position(block->org_x, block->org_y, current_rotation, 1999);
                move_block(the_level, rot_pos.x, rot_pos.y, ret_pos.x, ret_pos.y);
            }
            else if(pattern[block->move_counter] != MOVE_WAIT)
            {
                if(pattern[block->move_counter] == MOVE_RIGHT)
                {
                    Block* block_on_right = get_block(the_level, block->x+1, block->y);
                    if(block_on_right != NULL && block_on_right->move_timer <= 1)
                    {
                        block_update(block_on_right);
                    }
                }
                push_block(the_level, (pattern[block->move_counter]+current_rotation) % 4, rot_pos.x, rot_pos.y);
            }

            block->move_counter2++;
            if(block->move_counter2 >= pattern[block->move_counter+1])
            {
                block->move_counter2 = 0;
                block->move_counter = (block->move_counter + 2) % (block_table[block->block_code].move_pattern_len*2);
            }
        }
    }

    if(block->freeze_timer > 0)
    {
        block->freeze_timer--;
        if(block->freeze_timer <= 20)
        {
            block->frozen = false;
        }
    }

    if(block->teleport_timer > 0)
    {
        block->teleport_timer--;
    }

    if(block->can_vanish)
    {
        if(!block_is_active(block, NULL) && is_player_at(the_level, get_block_x(block), get_block_y(block)+1))
        {
            block->vanish_timer = block->vanish_len2;
        }
        block->vanish_timer += block->vanish_timer_sign;
        if(block->vanish_timer >= block->vanish_len2)
        {
            block->vanish_timer_sign = -1;
        }
        if(block->vanish_timer < 0)
        {
            block->vanish_timer_sign = 0;
            block->vanish_timer = 0;
        }
    }
}

void use_teleport_block(Block* block, Player* pl)
{
    if(block->teleport_timer == 0)
    {
        Block* tele = find_teleport_of_color(block->x, block->y, block->color);
        if(tele != NULL)
        {
            tele->teleport_timer = 54;
            block->teleport_timer = 54;
            pl->x += (tele->x - block->x) * BLOCK_W;
            pl->y += (tele->y - block->y) * BLOCK_H;
            play_sound(snd_teleport);
            start_anim(anmd_teleport, 193, 111, false, true);
        }
    }
}

void block_vanish(Block* block)
{
    block->vanish_timer_sign = 1;
}

void block_throw_pieces(Block* block, int count)
{
    int _loc3_ = 0;
    float _loc5_;
    float _loc6_;
    Block_piece* _loc7_;
    double2 _loc2_ = (double2) {get_block_pos_x(block), get_block_pos_y(block)};
    while(_loc3_ < count)
    {
        _loc5_ = random_float() * 30 + _loc2_.x;
        _loc6_ = random_float() * 30 + _loc2_.y;
        _loc7_ = create_block_piece(get_block_texture_by_id(block->block_code), 0.75,0.95,0.05,10,10,25,_loc5_,_loc6_);
        add_block_piece(_loc7_);
        _loc3_++;
    }
}

void block_break(Block* block)
{
    if(block == NULL) return;

    play_sound(snd_block_break);
    block_throw_pieces(block, 6);
    set_block(the_level, get_block_x(block), get_block_y(block), 0);
}

void block_explode(Block* block)
{
    play_sound(snd_explode);
    start_anim(anmd_explode, block->pos_x, block->pos_y, false, false);
    set_block(the_level, get_block_x(block), get_block_y(block), 0);
}

void hit_block_with_sword(Block* block)
{
    if(block == NULL) return;

    switch(block_table[block->block_code].sword_reaction)
    {
        case IR_SHATTER: block_break(block); break;
        case IR_EXPLODE: block_explode(block); return;
    }
}

void block_on_touch_mine(Block* block, Player* pl)
{
    number_t _loc2_;
    number_t _loc3_;
    number_t _loc4_;
    number_t _loc5_;
    number_t _loc6_;
    if(!block->frozen)
    {
        _loc2_ = pl->x - (get_block_pos_x(block) + 15);
        _loc3_ = pl->y - pl->var_325 / 2 - (get_block_pos_y(block) + 15);
        _loc4_ = atan2(_loc3_,_loc2_);
        _loc5_ = cos(_loc4_) * 50;
        _loc6_ = sin(_loc4_) * 50;
        player_hit(pl, _loc5_, _loc6_);
        block_explode(block);
    }
}

number_t get_block_pos_x(Block* block)
{
    switch(current_rotation)
    {
        case ROT_0: return block->pos_x;
        case ROT_90: return 1999*BLOCK_W - block->pos_y;
        case ROT_180: return 1999*BLOCK_W - block->pos_x;
        case ROT_270: return block->pos_y;
    }
    return block->pos_x;
}
number_t get_block_pos_y(Block* block)
{
    switch(current_rotation)
    {
        case ROT_0: return block->pos_y;
        case ROT_90: return block->pos_x;
        case ROT_180: return 1999*BLOCK_W - block->pos_y;
        case ROT_270: return 1999*BLOCK_W - block->pos_x;
    }
}
int get_block_x(Block* block)
{
    switch(current_rotation)
    {
        case ROT_0: return block->x;
        case ROT_90: return 1999 - block->y;
        case ROT_180: return 1999 - block->x;
        case ROT_270: return block->y;
    }
    return block->pos_x;
}
int get_block_y(Block* block)
{
    switch(current_rotation)
    {
        case ROT_0: return block->y;
        case ROT_90: return block->x;
        case ROT_180: return 1999 - block->y;
        case ROT_270: return 1999-block->x;
    }
}
