#include <allegro5/allegro.h>
#include <allegro5/allegro_opengl.h>
//#include <GLES2/gl2.h>
#include <math.h>
#include <stdio.h>
#include "player.h"
#include "level.h"
#include "tools.h"
#include "main.h"
#include "sounds.h"
#include "render.h"
#include "graph.h"
#include "debug.h"
#include "replay.h"
#include "control2.h"

Player the_player;

const float pl_dist_head_body = 27.0;
const float pl_dist_body_feet = 17.0;
const float pl_dist_center_down = 22.0;
const float pl_dist_head_body_fr = 27.0;
const float pl_dist_body_feet_fr = 17.0;
const float pl_dist_center_down_fr = 22.0;
const float pl_dist_head_body_c = 17.0;
const float pl_dist_body_feet_c = 7.0;
const float pl_dist_center_down_c = 12.0;
const float pl_width = 0.5;
const float pl_height = 1.49;

void player_init(Player* pl)
{
    memset(pl, 0, sizeof(Player));

    pl->mode = PLAYER_MODE_LAND;
    pl->x = 200;
    pl->y = 60;
    pl->vel_x = 0;
    pl->vel_y = 0;

    pl->var_157 = 28;
    pl->friction = 0.988; //0.985
    pl->var_150 = 0;
    pl->var_189 = 10;
    pl->var_325 = 55;
    pl->var_523 = 0.35;
    pl->var_599 = 1;
    pl->var_147 = pl->var_523;
    pl->var_524 = pl->var_599;

    pl->held_item = ITEM_NONE;
    pl->second_item = ITEM_NONE;

    pl->hats = 1 << HAT_MINESWEEPER;
    player_set_stats(pl, 100, 45, 60, true);

    pl->tex_head = gtex_load_texture("assets/img/body/head_0.png");
    pl->tex_body = gtex_load_texture("assets/img/body/body_0.png");
    pl->tex_foot = gtex_load_texture("assets/img/body/foot_0.png");
}

void player_set_stats(Player* pl, int param1, int param2, int param3, bool param4)
{
    pl->speed_stat = num_limit(param1, 0, 100);
    pl->accel_stat = num_limit(param2, 0, 100);
    pl->jumpn_stat = num_limit(param3, 0, 100);
    if(pl->speed_burst_timer <= 0 || param4)
    {
        pl->max_vel_x = 2.0 + pl->speed_stat / 10.0;
        pl->accel = 0.2 + pl->accel_stat / 60.0;
        if(player_has_hat(pl, HAT_SANTA))
        {
            pl->max_vel_x += 1;
        }
    }

    pl->super_jump = 2 + pl->jumpn_stat / 40.0;
    /*if(the_level != null && Course.course._statsDisplay != null)
    {
        Course.course._statsDisplay.set_stats(pl->speed_stat,pl->accel_stat,pl->jumpn_stat);
    }*/
    /*if(pl->starting_stats == null && Course.course != null)
    {
        pl->starting_stats = [pl->speed_stat,pl->accel_stat,pl->jumpn_stat];
    }*/
}

void player_set_mode(Player* pl, enum Player_mode mode)
{
    if(pl->mode != mode)
    {
        pl->mode = mode;
        pl->var_24 = 0;
        if(pl->mode == PLAYER_MODE_HURT)
        {
            player_change_state(pl, PLAYER_STATE_BUMPED);
            //TODO pl->bumpPlayer();
        }
        if(pl->mode == PLAYER_MODE_WATER && pl->state != PLAYER_STATE_BUMPED)
        {
            player_change_state(pl, PLAYER_STATE_SWIM);
        }
        if(pl->mode == PLAYER_MODE_SQUASHED)
        {
            //TODO pl->squashedTime = 60;
            //method_51(70);
        }
    }
}

void player_get_keys_from_replay_frame(Player* pl, Extended_list* replay, int frame)
{
    byte keys;
    exl_get_elm(replay, frame, &keys);

    if(keys & 1)
    {
        pl->up = true;
    }
    else pl->up = false;

    if(keys & 2)
    {
        pl->down = true;
    }
    else pl->down = false;

    if(keys & 4)
    {
        pl->left = true;
        pl->direction = 0;
    }
    else pl->left = false;

    if(keys & 8)
    {
        pl->right = true;
        pl->direction = 1;
    }
    else pl->right = false;

    if((keys & 16) && pl->item_lock)
    {
        use_item(pl);
        pl->item_lock = false;
    }
    if(!(keys & 16)) pl->item_lock = true;
}

void player_update_keys(Player* pl)
{
    if(al_key_down(&g_kstate, ALLEGRO_KEY_UP))
    {
        pl->up = true;
    }
    else
    {
        pl->up = false;
        reset_bump_sound();
    }

    if(al_key_down(&g_kstate, ALLEGRO_KEY_DOWN))
    {
        pl->down = true;
    }
    else pl->down = false;

    if(al_key_down(&g_kstate, ALLEGRO_KEY_LEFT))
    {
        pl->left = true;
        pl->direction = 0;
    }
    else pl->left = false;

    if(al_key_down(&g_kstate, ALLEGRO_KEY_RIGHT))
    {
        pl->right = true;
        pl->direction = 1;
    }
    else pl->right = false;

    pl->ctrl = al_key_down(&g_kstate, ALLEGRO_KEY_RCTRL);

    if(al_key_down(&g_kstate, ALLEGRO_KEY_SPACE) && pl->item_lock)
    {
        use_item(pl);
        pl->item_lock = false;
    }
    if(!al_key_down(&g_kstate, ALLEGRO_KEY_SPACE)) pl->item_lock = true;
}

void player_update(Player* pl)
{
    if(pl->rotating > 0.0)
    {
        pl->rotating -= 0.04;
        if(pl->rotating < 0.0)
        {
            pl->rotating = 0;
        }
        return;
    }
    else if(pl->rotating < 0.0)
    {
        pl->rotating += 0.04;
        if(pl->rotating > 0.0)
        {
            pl->rotating = 0;
        }
        return;
    }

    if(al_key_down(&g_kstate, ALLEGRO_KEY_ENTER)) {
        g_replay_mode = 2;
    }
    else if(al_key_down(&g_kstate, ALLEGRO_KEY_RSHIFT)) {
        g_replay_mode = 1;
    }

    static int frame_nr = 0;
    if(g_replay_mode == 2) {
        player_get_keys_from_replay_frame(pl, bound_replay, frame_nr++);
    } else {
        player_update_keys(pl);
    }

    if(g_replay_mode == 1) {
        replay_add_frame(bound_replay, pl->up, pl->down, pl->left, pl->right, al_key_down(&g_kstate, ALLEGRO_KEY_SPACE));
    }

    switch(pl->mode)
    {
        case PLAYER_MODE_LAND: player_land_go(pl); break;
        case PLAYER_MODE_WATER: player_water_go(pl); break;
    }

    if(pl->ctrl)
    {
        if(player_has_hat(pl, HAT_MINESWEEPER))
        {
            Block* block = pr2_get_block_from_pos(pl->direction ? pl->x+30 : pl->x-30, pl->y-5, false);
            if(block != NULL && block->block_code == BL_MINE && pl->held_item != ITEM_MINE)
            {
                give_item_to_player(pl, ITEM_MINE);
                set_block(the_level, block->x, block->y, 0);
            }
        }
    }

    if(pl->speed_burst_timer > 0)
    {
        pl->speed_burst_timer--;
        if(pl->speed_burst_timer <= 0)
        {
            pl->accel *= 0.5;
            pl->max_vel_x *= 0.5;
            play_sound(snd_speed_end);
        }
    }

    if(!pl->on_ground && pl->mode != PLAYER_MODE_WATER)
    {
        reset_bump_sound();
    }
}

void player_change_state(Player* pl, enum Player_state state)
{
    pl->state = state;
}

void player_position(Player* pl)
{
    number_t gravity_multiplied = g_default_gravity * the_level->gravity_multiplier;
    number_t _loc1_;
    double2 _loc2_;
    int _loc3_;
    if(player_has_hat(pl, HAT_MOON))
    {
        gravity_multiplied *= 0.85;
    }
    pl->vel_y += gravity_multiplied;

    if(pl->up && player_has_hat(pl, HAT_PROPELLER) && pl->vel_y > 0)
    {
        pl->vel_y *= 0.85;
    }

    pl->var_24 *= pl->friction;
    if(pl->crawling)
    {
        pl->var_24 *= 0.7;
    }
    pl->var_24 = num_limit(pl->var_24, -pl->max_vel_x, pl->max_vel_x);
    _loc1_ = fabs(pl->vel_x) / pl->var_157;
    _loc1_ = 1 - _loc1_;
    _loc1_ *= 0.9;
    _loc1_ += 0.1;
    pl->var_147 *= pl->frozen_solid ? 0 : _loc1_;
    pl->vel_x += (pl->var_24 - pl->vel_x) * pl->var_147;
    pl->vel_x = num_limit(pl->vel_x,-pl->var_157,pl->var_157);
    pl->vel_y = num_limit(pl->vel_y,-pl->var_157,pl->var_157);
    pl->x += pl->vel_x;
    pl->y += pl->vel_y;
    //TODO rotate

    if(pl->y > get_level_max_y()*BLOCK_H+500)
    {
        player_return_to_safe(pl);
    }

    pl->var_147 = pl->var_523;
    pl->var_524 = pl->var_599;
}

void player_water_go(Player* pl)
{
    if(pl->right)
    {
        pl->vel_x += pl->accel * 0.5;
    }
    if(pl->left)
    {
        pl->vel_x -= pl->accel * 0.5;
    }
    if(pl->down)
    {
        pl->vel_y += pl->accel * 0.65;
    }
    if(pl->up)
    {
        pl->vel_y -= pl->accel * 0.65;
    }
    pl->vel_y += g_default_gravity * 0.25;
    pl->vel_x *= 0.92;
    pl->vel_y *= 0.92;
    pl->vel_x = num_limit(pl->vel_x,-pl->var_157,pl->var_157);
    pl->vel_y = num_limit(pl->vel_y,-pl->var_157,pl->var_157);
    pl->x += pl->vel_x;
    pl->y += pl->vel_y;

    player_handle_collisions(pl);

    --pl->var_240;
    if(player_has_hat(pl, HAT_COWBOY) && !pl->on_ground)
    {
        pl->var_240 = 1;
    }
    if(pl->var_240 <= 0)
    {
        if(pl->up)
        {
            pl->vel_y -= pl->super_jump * 0.5;
            pl->const_12 = -pl->super_jump * 0.5;
            pl->var_281 = true;
        }
        player_set_mode(pl, PLAYER_MODE_LAND);
    }
}

void player_land_go(Player* pl)
{
    if(pl->right)
    {
        pl->var_24 += pl->accel;
    }
    if(pl->left)
    {
        pl->var_24 -= pl->accel;
    }
    if(!pl->right && !pl->left)
    {
        pl->var_24 = 0;
    }

    if(pl->up)
    {
        if(pl->on_ground && !pl->crawling)
        {
            pl->var_281 = true;
            pl->vel_y -= pl->super_jump;
            pl->const_12 = -pl->super_jump;
            play_sound(snd_jump);
        }
        else if(pl->var_281)
        {
            pl->vel_y += pl->const_12;
            pl->const_12 *= 0.75;
        }
    }
    else
    {
        pl->var_281 = false;
    }
    if(pl->down)
    {
        if(!pl->crawling)
        {
            if(!pl->on_ground)
            {
                pl->vel_y += 0.5;
                pl->var_150 = 0;
            }
            else
            {
                if(pl->var_150 < 100)
                {
                    pl->var_150 += 2;
                }
                if(pl->var_150 > 25)
                {
                    pl->var_24 = 0;
                }
            }
        }
    }
    else
    {
        if(pl->var_150 > 25)
        {
            pl->vel_y = -pl->var_150 * 0.24;
            pl->var_281 = false;
            play_sound(snd_sjump);
        }
        pl->var_150 = 0;
    }

    player_position(pl);

    //TODO scale
    if(!pl->on_ground)
    {
        player_change_state(pl, PLAYER_STATE_JUMP);
    }
    else if(pl->var_150 > 25)
    {
        player_change_state(pl, PLAYER_STATE_SUPER_JUMP);
    }
    else if(pl->left || pl->right)
    {
        player_change_state(pl, pl->crawling ? PLAYER_STATE_CRAWL_WALK : PLAYER_STATE_RUN);
    }
    else
    {
        player_change_state(pl, pl->crawling ? PLAYER_STATE_CRAWL : PLAYER_STATE_STAND);
    }

    player_handle_collisions(pl);
    //TODO cowboy
}

void player_method_261(Player* pl)
{
    if(get_block_action(pl, pl->var_469, SIDE_TOP, current_rotation) != BA_INACTIVE && get_block_action(pl, pl->var_262, SIDE_BOTTOM, current_rotation) == BA_INACTIVE)
    {
        block_on_stand(pl->var_469, pl);
        player_set_neighboring_blocks(pl);
        pl->on_ground = true;
    }
    else
    {
        pl->on_ground = false;
    }
}

void player_handle_collisions(Player* pl)
{
    Block* _loc1_ = NULL;
    Block* _loc2_ = NULL;
    Block* _loc3_ = NULL;
    number_t _loc4_;

    player_set_neighboring_blocks(pl);
    player_method_261(pl);
    if(player_has_hat(pl, HAT_SANTA))
    {
        _loc3_ = pr2_get_block_from_pos(pl->x, pl->y, true);
        if(_loc3_ != NULL && (get_block_type(_loc3_) == BT_WATER && pl->mode != PLAYER_MODE_WATER || get_block_type(_loc3_) == BT_NET))
        {
            if(_loc3_->frozen || _loc3_->freeze_timer == 0)
            {
                block_on_stand(_loc3_, pl);
            }
        }
    }

    if(pl->vel_x >= -1)
    {
        if(pl->var_296 != NULL)
        {
            Block* b2 = pr2_get_block(get_block_pos_x(pl->var_296) - 30, get_block_pos_y(pl->var_296), true, false);
            if(get_block_action(pl, b2, SIDE_RIGHT, current_rotation) == BA_INACTIVE)
            {
                if((pl->var_296->frozen && pl->var_296->freeze_timer < 100) || get_block_action(pl, pl->var_296, SIDE_LEFT, current_rotation) != BA_INACTIVE)
                {
                    block_on_left_hit(pl->var_296, pl);
                    player_set_neighboring_blocks(pl);
                }
            }
        }
    }
    if(pl->vel_x <= 1)
    {
        if(pl->var_329 != NULL)
        {
            Block* b2 = pr2_get_block(get_block_pos_x(pl->var_329) + 30, get_block_pos_y(pl->var_329), true, false);
            if(get_block_action(pl, b2, SIDE_LEFT, current_rotation) == BA_INACTIVE)
            {
                if((pl->var_329->frozen && pl->var_329->freeze_timer < 100) ||  get_block_action(pl, pl->var_329, SIDE_RIGHT, current_rotation) != BA_INACTIVE)
                {
                    block_on_right_hit(pl->var_329, pl);
                    player_set_neighboring_blocks(pl);
                }
            }
        }
    }
    if(pl->vel_y < 0)
    {
        if(pl->on_ground)
        {
            pl->crawling = true;
        }
        if(pl->mode != PLAYER_MODE_WATER && get_block_action(pl, pl->var_262, SIDE_BOTTOM, current_rotation) != BA_INACTIVE && get_block_action(pl, pr2_get_block(get_block_pos_x(pl->var_262), get_block_pos_y(pl->var_262) + 30, true, false), SIDE_TOP, current_rotation) == BA_INACTIVE)
        {
            block_on_bump(pl->var_262, pl);
            player_set_neighboring_blocks(pl);
        }
        else if(pl->mode != PLAYER_MODE_WATER && get_block_action(pl, pl->var_306, SIDE_BOTTOM, current_rotation) != BA_INACTIVE && get_block_action(pl, pr2_get_block(get_block_pos_x(pl->var_306), get_block_pos_y(pl->var_306) + 30, true, false), SIDE_TOP, current_rotation) == BA_INACTIVE)
        {
            block_on_bump(pl->var_306, pl);
            player_set_neighboring_blocks(pl);
        }
        else if(get_block_action(pl, pl->var_297, SIDE_BOTTOM, current_rotation) != BA_INACTIVE && get_block_action(pl, pr2_get_block(get_block_pos_x(pl->var_297), get_block_pos_y(pl->var_297) + 30, true, false), SIDE_TOP, current_rotation) == BA_INACTIVE)
        {
            block_on_bump(pl->var_297, pl);
            player_set_neighboring_blocks(pl);
        }
    }
    if(!pl->on_ground)
    {
        player_method_261(pl);
    }
    _loc1_ = NULL;
    _loc2_ = NULL;
    pl->crawling = false;
    if(pl->on_ground == true)
    {
        _loc1_ = pr2_get_block(pl->x, pl->y - 40, true, false);
        _loc2_ = pr2_get_block(pl->x, pl->y - 10, true, false);
        if(get_block_action(pl, _loc1_, SIDE_BOTTOM, pl->rot) != BA_INACTIVE && get_block_action(pl, _loc2_, SIDE_TOP, pl->rot) == BA_INACTIVE)
        {
            pl->crawling = true;
            if(pl->up)
            {
                _loc4_ = pl->y;
                block_on_bump(_loc1_, pl);
                pl->y = !(/*TODO _loc1_ is TeleportBlock*/false) ? _loc4_ : pl->y;
                pl->vel_y = 0;
            }
            if(pl->vel_y < 0)
            {
                pl->vel_y = 0;
            }
        }
    }
    _loc1_ = pr2_get_block_from_pos(pl->x, pl->y - 15,true);
    if(_loc1_ != NULL)
    {
        block_on_touch(_loc1_, pl);
    }
    if(!pl->crawling)
    {
        _loc1_ = pr2_get_block_from_pos(pl->x, pl->y - 45,true);
        if(_loc1_ != NULL)
        {
            block_on_touch(_loc1_, pl);
        }
    }

}

void player_set_neighboring_blocks(Player* pl)
{
    if(pl->y < 0)
    {
        pl->y += 0.001;
    }

    pl->var_630 = pr2_get_block(pl->x - pl->var_189,pl->y,true,true);
    pl->var_469 = pr2_get_block(pl->x,pl->y,true,true);
    pl->var_657 = pr2_get_block(pl->x + pl->var_189,pl->y,true,true);
    pl->var_329 = pr2_get_block(pl->x - pl->var_189,pl->y - 10, true, false);
    pl->var_658 = pr2_get_block(pl->x,pl->y - 10, true, false);
    pl->var_296 = pr2_get_block(pl->x + pl->var_189,pl->y - 10, true, false);
    pl->var_654 = pr2_get_block(pl->x - pl->var_189,pl->y - 30, true, false);
    //pl->var_262 = pr2_get_block(pl->x + (pl->direction ? 0.45 : -0.4), pl->y - 30, true, false);
    pl->var_262 = pr2_get_block(pl->x, pl->y - 30, true, false);
    pl->var_631 = pr2_get_block(pl->x + pl->var_189,pl->y - 30, true, false);
    pl->var_306 = pr2_get_block(pl->x,pl->y - pl->var_325 + 30, true, false);
    pl->var_297 = pr2_get_block(pl->x,pl->y - pl->var_325, true, false);


    /*if(pl->var_469->block_code != 0)
    {
        printf("pos %f %f\n", pl->x, pl->y);
        exit(0);
    }*/
}

void player_return_to_safe(Player* pl)
{
    pl->x = pl->safe_x;
    pl->y = pl->safe_y;
    pl->vel_x = 0;
    pl->vel_y = 0;
}

void player_hit(Player* pl, number_t param1, number_t param2)
{
    //var _loc3_:Object = null;
    //TODO if((!var_4.getBool(CROWN) || this.course.gameMode == Modes.dm || this.course.gameMode == Modes.hat) && !this.invincible)
    {
        pl->vel_x += param1;
        pl->vel_y += param2;
        if(!player_has_hat(pl, 0/*TODO*/))
        {
            //method_51(50);
            /*if(!this.frozenSolid)
            {
                this.setMode("hurt");
            }*/
        }
        /*TODO if(this.map != null && !this.testMode)
        {
            _loc3_ = getHighestHat();
            if(_loc3_.hatNum != 1 && _loc3_.hatNum != 0 && _loc3_.hatNum != null)
            {
                Main.socket.write("loose_hat`" + x + "`" + (y - 50) + "`" + this.map.rotation);
            }
        }*/
    }
}

bool player_has_hat(Player* pl, int hat)
{
    if(pl == NULL) return false;
    return pl->hats & (1 << hat);
}

void rotate_player(Player* pl, enum Rotation rot)
{
    pl->rot = (pl->rot + rot) % 4;
    current_rotation = pl->rot;

    int2 new_pos = rotate_position(pl->x, pl->y, rot, 2000*30-1);
    printf("rot %lf %lf, %i %i %i\n", pl->x/30, pl->y/30, new_pos.x/30, new_pos.y/30, pl->rot);
    //exit(1);
    if(rot == ROT_90)
    {
        pl->x = new_pos.x+6;
        pl->y = new_pos.y+15;
    }
    else if(rot == ROT_270)
    {
        pl->x = new_pos.x-15;
        pl->y = new_pos.y+5;
    }

    if(rot == ROT_270)
    {
        pl->rotating = 1.570796327;
    }
    else if(rot == ROT_90)
    {
        pl->rotating = -1.570796327;
    }
}

void render_player(Player* pl, float x_screen, float y_screen)
{
	static float scale_val = 0.2; //0.4
    static bool lock = true;

    glColor3f(1, 1, 1 - pl->var_150 / 100.0);

    glEnable(GL_TEXTURE_2D);
    float2 player_pos = {x_screen, y_screen};

    if(pl->crawling)
    {
        y_screen += BLOCK_H/3.0;
    }

    /*if(pl->dm_anim)
    {
        const float sit1_lim_a = -0.6;
        const float sit1_lim_y = 0.0375;
        int dac = pl->dm_anim_counter;
        float sp = player_wears_hat(pl, HAT_NURSE) ? 1 : 0.5;

        if(pl->dir)
        {
            if((dac/10) % 2)
            {
                glColor4f(1, 1, 1, 0.75);
            }
            else glColor4f(1, 1, 1, 1);

            //body
            render_texture_fr(pl->tex_body, pl->a_body_center.x, pl->a_body_center.y, scale_val, !pl->dir, pl->a_body_center.x*DEF_GUI_W.0, pl->a_body_center.y*DEF_GUI_H.0, pl->a_body_rot);

            if(dac < 40)
            {
                pl->a_body_center.y += 1/30.0 * sit1_lim_y * sp;
                pl->a_body_rot += 1/20.0 * sit1_lim_a * sp;
            }
            else if(dac < 50)
            {
                pl->a_body_rot += 1/20.0 * sit1_lim_a * sp;
            }
            else if(dac < 60)
            {
                pl->a_body_rot -= 1/20.0 * sit1_lim_a * sp;
            }
            else if(dac < 100)
            {
                pl->a_body_center.y -= 1/30.0 * sit1_lim_y * sp;
                pl->a_body_rot -= 1/20.0 * sit1_lim_a * sp;
            }

            //feet
            render_texture_fr(pl->tex_foot, pl->a_feet_center.x, pl->a_feet_center.y, scale_val, !pl->dir, pl->a_feet_center.x*DEF_GUI_W.0, pl->a_feet_center.y*DEF_GUI_H.0, pl->a_feet_rot);

            if(dac < 40)
            {
                pl->a_feet_center.x += 1.2/30.0 * sit1_lim_y * sp;
                pl->a_feet_center.y -= 1/80.0 * sit1_lim_y * sp;
                pl->a_feet_rot += 2.1/20.0 * sit1_lim_a * sp;
            }
            else if(dac >= 60 && dac < 100)
            {
                pl->a_feet_center.x -= 1.2/30.0 * sit1_lim_y * sp;
                pl->a_feet_center.y += 1/80.0 * sit1_lim_y * sp;
                pl->a_feet_rot -= 2.1/20.0 * sit1_lim_a * sp;
            }

            //head
            render_texture_fr(pl->tex_head, pl->a_head_center.x, pl->a_head_center.y, scale_val, !pl->dir, pl->a_head_center.x*DEF_GUI_W.0, pl->a_head_center.y*DEF_GUI_H.0, pl->a_head_rot);

            if(dac < 20 / 0.5)
            {
                pl->a_head_center.x -= 0.0004 * sp;
                pl->a_head_center.y += 0.0016 * sp;
            }
            else if(dac < 25 / 0.5)
            {
                pl->a_head_center.x -= 0.003 * sp;
                pl->a_head_rot -= 0.1 * sp;
            }
            else if(dac < 30 / 0.5)
            {
                pl->a_head_center.x += 0.003 * sp;
                pl->a_head_rot += 0.1 * sp;
            }
            else if(dac < 50 / 0.5)
            {
                pl->a_head_center.x += 0.0004 * sp;
                pl->a_head_center.y -= 0.0016 * sp;
            }
        }
        else
        {
            if((dac/10) % 2)
            {
                glColor4f(1, 1, 1, 0.75);
            }
            else glColor4f(1, 1, 1, 1);

            //body
            render_texture_fr(pl->tex_body, pl->a_body_center.x, pl->a_body_center.y, scale_val, !pl->dir, pl->a_body_center.x*DEF_GUI_W.0, pl->a_body_center.y*DEF_GUI_H.0, pl->a_body_rot);

            if(dac < 40)
            {
                pl->a_body_center.y += 1/30.0 * sit1_lim_y * sp;
                pl->a_body_rot -= 1/20.0 * sit1_lim_a * sp;
            }
            else if(dac < 50)
            {
                pl->a_body_rot -= 1/20.0 * sit1_lim_a * sp;
            }
            else if(dac < 60)
            {
                pl->a_body_rot += 1/20.0 * sit1_lim_a * sp;
            }
            else if(dac < 100)
            {
                pl->a_body_center.y -= 1/30.0 * sit1_lim_y * sp;
                pl->a_body_rot += 1/20.0 * sit1_lim_a * sp;
            }

            //feet
            render_texture_fr(pl->tex_foot, pl->a_feet_center.x, pl->a_feet_center.y, scale_val, !pl->dir, pl->a_feet_center.x*DEF_GUI_W.0, pl->a_feet_center.y*DEF_GUI_H.0, pl->a_feet_rot);

            if(dac < 40)
            {
                pl->a_feet_center.x -= 1.2/30.0 * sit1_lim_y * sp;
                pl->a_feet_center.y -= 1/80.0 * sit1_lim_y * sp;
                pl->a_feet_rot -= 2.1/20.0 * sit1_lim_a * sp;
            }
            else if(dac >= 60 && dac < 100)
            {
                pl->a_feet_center.x += 1.2/30.0 * sit1_lim_y * sp;
                pl->a_feet_center.y += 1/80.0 * sit1_lim_y * sp;
                pl->a_feet_rot += 2.1/20.0 * sit1_lim_a * sp;
            }

            //head
            render_texture_fr(pl->tex_head, pl->a_head_center.x, pl->a_head_center.y, scale_val, !pl->dir, pl->a_head_center.x*DEF_GUI_W.0, pl->a_head_center.y*DEF_GUI_H.0, pl->a_head_rot);

            if(dac < 20 / sp)
            {
                pl->a_head_center.x += 0.0004 * sp;
                pl->a_head_center.y += 0.0016 * sp;
            }
            else if(dac < 25 / sp)
            {
                pl->a_head_center.x += 0.003 * sp;
                pl->a_head_rot += 0.1 * sp;
            }
            else if(dac < 30 / sp)
            {
                pl->a_head_center.x -= 0.003 * sp;
                pl->a_head_rot -= 0.1 * sp;
            }
            else if(dac < 50 / sp)
            {
                pl->a_head_center.x -= 0.0004 * sp;
                pl->a_head_center.y -= 0.0016 * sp;
            }
        }

        pl->dm_anim_counter += player_wears_hat(pl, HAT_NURSE) ? 2 : 1;
        if(pl->dm_anim_counter >= 100)
        {
            pl->dm_anim = false;
            pl->dm_anim_counter = 0;
        }
    }
    else*/
    {
        if(pl->crawling)
        {
            render_texture(pl->tex_body, x_screen, y_screen, scale_val, !pl->direction);
            render_texture(pl->tex_head, x_screen, y_screen-pl_dist_head_body_c, scale_val, !pl->direction);

            if(pl->left || pl->right)
            {

                render_texture(pl->tex_foot, x_screen,                  y_screen+pl_dist_body_feet_c-pl->feet_counter*BLOCK_H/900.0, scale_val, !pl->direction);
                render_texture(pl->tex_foot, x_screen+BLOCK_W/9.0, y_screen+pl_dist_body_feet_c+pl->feet_counter*BLOCK_H/900.0, scale_val, !pl->direction);
            }
            else
            {
                render_texture(pl->tex_foot, x_screen,                  y_screen+pl_dist_body_feet_c, scale_val, !pl->direction);
                render_texture(pl->tex_foot, x_screen+BLOCK_W/9.0, y_screen+pl_dist_body_feet_c, scale_val, !pl->direction);
            }
        }
        else
        {
            render_texture(pl->tex_head, x_screen, y_screen-pl_dist_head_body, scale_val, !pl->direction);
            render_texture(pl->tex_body, x_screen, y_screen, scale_val, !pl->direction);

            if(pl->on_ground)
            {
                if(pl->left || pl->right)
                {
                    render_texture_fr(pl->tex_foot, x_screen+30.0/9.0, y_screen+pl_dist_body_feet, scale_val, !pl->direction, player_pos.x, player_pos.y, rad(-pl->feet_counter));
                    render_texture_fr(pl->tex_foot, x_screen, y_screen+pl_dist_body_feet, scale_val, !pl->direction, player_pos.x, player_pos.y, rad(pl->feet_counter));
                }
                else
                {
                    render_texture(pl->tex_foot, x_screen+30.0/9.0, y_screen+pl_dist_body_feet, scale_val, !pl->direction);
                    render_texture(pl->tex_foot, x_screen, y_screen+pl_dist_body_feet, scale_val, !pl->direction);
                }
            }
            else
            {
                render_texture_fr(pl->tex_foot, x_screen+30.0/9.0, y_screen+pl_dist_body_feet, scale_val, !pl->direction, player_pos.x, player_pos.y, rad(-pl->feet_counter));
                render_texture_fr(pl->tex_foot, x_screen, y_screen+pl_dist_body_feet, scale_val, !pl->direction, player_pos.x, player_pos.y, rad(pl->feet_counter));
            }
        }

        if(pl->on_ground)
        {
            pl->feet_counter += pl->feet_sign*6;
            if(pl->feet_counter > 44)
            {
                pl->feet_sign = -1;
            }
            else if(pl->feet_counter < 1)
            {
                pl->feet_sign = 1;
            }
        }
        else
        {
            if(pl->feet_counter < 45)
            {
                pl->feet_counter += 6;
            }
        }
    }
    glColor4f(1, 1, 1, 1);

    //hat
    if(player_has_hat(pl, HAT_SANTA))
    {
        static float color = 0;
        int r, g, b;
        hsv_to_rgb(fmodf(color, 360), 1, 1, &r, &g, &b);

        color += 1;
        render_texture(tex_hat_santa[0], x_screen + (pl->direction ? -6.7 : 6.7), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 8.3, scale_val*var_scale, !pl->direction);
        glColor3ub(r, g, b);
        render_texture(tex_hat_santa[1], x_screen + (pl->direction ? -9 : 9), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 10.8, scale_val*var_scale, !pl->direction);
        glColor3f(1, 1, 1);
        render_texture(tex_hat_santa[3], x_screen + (pl->direction ? -6.6 : 6.6), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 8.2, scale_val*var_scale, !pl->direction);
    }
    else if(player_has_hat(pl, HAT_PROPELLER))
    {
        render_texture(tex_hat_propeller, x_screen + (pl->direction ? -0.005*DEF_WIN_W : 0.005*DEF_WIN_W), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 0.016*DEF_WIN_H, scale_val*1.5, !pl->direction);
    }
    else if(player_has_hat(pl, HAT_CHEESE))
    {
        render_texture(tex_hat_cheese, x_screen + (pl->direction ? -0.005*DEF_WIN_W : 0.005*DEF_WIN_W), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 0.009*DEF_WIN_H, scale_val*1.5, !pl->direction);
    }
    else if(player_has_hat(pl, HAT_MINESWEEPER))
    {
        glColor3f(0, 0, 1);
        render_texture(tex_hat_cheese, x_screen + (pl->direction ? -0.005*DEF_WIN_W : 0.005*DEF_WIN_W), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 0.009*DEF_WIN_H, scale_val*1.5, !pl->direction);
        glColor3f(1, 1, 1);
    }
    else if(player_has_hat(pl, HAT_TOP))
    {
        render_texture(tex_hat_top, x_screen + (pl->direction ? -0.007*DEF_WIN_W : 0.007*DEF_WIN_W), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 0.014*DEF_WIN_H, scale_val*1.5, !pl->direction);
    }
    else if(player_has_hat(pl, HAT_MOON))
    {
        glColor3ub(255, 255, 0);
        render_texture(tex_hat_moon[2], x_screen + (pl->direction ? -7 : 7), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 10.8+11.5, scale_val*var_scale, !pl->direction);
        render_texture(tex_hat_moon[0], x_screen + (pl->direction ? -6.7 : 6.7), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 8.3+9, scale_val*var_scale, !pl->direction);
        glColor3f(1, 1, 1);
        render_texture(tex_hat_moon[1], x_screen + (pl->direction ? -6.9 : 6.9), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 8.2+8.8, scale_val*var_scale, !pl->direction);
    }
    else if(player_has_hat(pl, HAT_ITEM))
    {
        glColor3ub(255, 255, 0);
        render_texture(tex_hat_item[2], x_screen + (pl->direction ? -7+2.3 : 7-2.3), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 10.8+11.5-9.3, scale_val*var_scale, !pl->direction);
        glColor3ub(255, 255, 255);
        render_texture(tex_hat_item[0], x_screen + (pl->direction ? -6.7+2.3 : 6.7-2.3), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 8.3+9-9.3, scale_val*var_scale, !pl->direction);
        glColor3f(1, 1, 1);
        render_texture(tex_hat_item[1], x_screen + (pl->direction ? -6.9+2.3 : 6.9-2.3), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 8.2+8.8-9.3, scale_val*var_scale, !pl->direction);
    }
    /*else if(player_wears_hat(pl, HAT_MHELMET))
    {
        render_texture(tex_hat_mhelmet, x_screen + (pl->direction ? -0.001 : 0.001), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 0.014, scale_val*1.25, !pl->direction);
    }
    else if(player_wears_hat(pl, HAT_NURSE))
    {
        render_texture(tex_hat_nurse, x_screen + (pl->direction ? -0.004 : 0.004), y_screen - (pl->crawling ? pl_dist_head_body_c : pl_dist_head_body) - 0.014, scale_val*1.5, !pl->direction);
    }*/
}
