#include <allegro5/allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include "level_editor.h"
#include "level.h"
#include "main.h"
#include "map.h"
#include "internet.h"
#include "player.h"
#include "render.h"
#include "graph.h"
#include "debug.h"
#include "control2.h"
#include "system_funcs.h"
#include "sounds.h"

#define left_bar_x 10
#define left_bar_y 80
#define left_bar_w 80
#define left_bar_h 80
#define top_bar_x 690
#define top_bar_y 10
#define top_bar_w 388
#define top_bar_h 82
#define menu_x (top_bar_x+166)
#define menu_y (top_bar_y+top_bar_h)
#define win_x 200
#define win_y 106
#define win_x2 (DEF_GUI_W-win_x)
#define win_y2 (DEF_GUI_H-win_y)
#define g_win_w (DEF_GUI_W-2*win_x)
#define g_win_h (DEF_GUI_H-2*win_y)

enum Opened_window
{
    LVLW_NONE,
    LVLW_LOAD
};

enum Section
{
    LS_BLOCKS,
    LS_ART,
    LS_SETTINGS
};

float scroll_speed = 20;
static int held_block = 1;
Menu* menu_lvl_edit;
enum Opened_window opened_win = LVLW_NONE;
enum Section section;
bool menu_shown;
static int selected_lvl = -1;
static Pointer_list* levels_list;

void init_level_editor()
{
    menu_lvl_edit = create_menu(7,
        "save", NULL,
        "load", open_load_lvl_window,
        "export", NULL,
        "import", NULL,
        "new", NULL,
        "goto block editor", NULL,
        "goto main menu", NULL
    );
}

void load_level_pr23(const char* filename)
{
    FILE* file = fopen(filename, "r");
    destroy_level(the_level);
    the_level = create_level();

    fscanf(file, "%i %i", &the_level->start_pos[0].x, &the_level->start_pos[0].y);
    while(true)
    {
        int x, y, id;
        if(fscanf(file, "%i %i %i", &x, &y, &id) != 3)
        {
            break;
        }
        set_block(the_level, x, y, id);
    }
    the_level->camera_x = the_level->start_pos[0].x*BLOCK_W;
    the_level->camera_y = the_level->start_pos[0].y*BLOCK_H;
    fclose(file);
}

void save_level(const char* filename)
{
    FILE* file = fopen(filename, "w");
    fprintf(file, "%i %i ", the_level->start_pos[0].x, the_level->start_pos[0].y);
    for(int y=0; y<2000; y++)
    {
        for(int x=0; x<2000; x++)
        {
            Block* block = get_block(the_level, x, y);
            if(block != NULL)
            {
                fprintf(file, "%i %i %i ", x, y, block->block_code);
            }
        }
    }
    fclose(file);
}

void download_level(int id)
{
    char url[64];
    char dest[32];
    sprintf(url, "https://pr2hub.com/levels/%i.txt", id);
    sprintf(dest, "levels/%i.txt", id);
    download_file(url, dest);
}

void enter_level_editor()
{
    destroy_level(the_level);
    the_level = create_level();
}

void render_level_editor()
{
    render_level(the_level, -the_level->camera_x, -the_level->camera_y);
    if(held_block != 0)
    {
        gtex_render_texture(get_block_texture_by_id(held_block), g_mstate.x/2.0-15, g_mstate.y/2.0-15, g_mstate.x/2.0+15, g_mstate.y/2.0+15);
    }
    render_lvl_edit_gui();
}

bool is_mouse_on_gui()
{
    if(opened_win != LVLW_NONE)
        return true;

    if(is_between(g_mouse_x, g_mouse_y, left_bar_x, left_bar_y, left_bar_x+left_bar_w, left_bar_y+left_bar_h))
        return true;

    if(is_between(g_mouse_x, g_mouse_y, top_bar_x, top_bar_y, top_bar_x+top_bar_w, top_bar_y+top_bar_h))
        return true;

    return false;
}

void lvl_editor_update()
{
    static bool lock = true;

    int mx = (g_mstate.x/2.0 + the_level->camera_x) / 30;
    int my = (g_mstate.y/2.0 + the_level->camera_y) / 30;

    if(!is_mouse_on_gui())
    {
        if(g_mstate.buttons == 1 &! al_key_down(&g_kstate, ALLEGRO_KEY_SPACE))
        {
            Block* bl = get_block(the_level, mx, my);
            if(bl == NULL)
            {
                set_block(the_level, mx, my, held_block);
                if(held_block == BL_TELEPORT)
                {
                    bl = get_block(the_level, mx, my);
                    bl->color = 0x00ff00;
                }
            }
        }
        else if(g_mstate.buttons == 2)
        {
            Block* bl = get_block(the_level, mx, my);
            if(bl != NULL)
            {
                set_block(the_level, mx, my, 0);
            }
        }
    }

    if(al_key_down(&g_kstate, ALLEGRO_KEY_UP))
    {
        the_level->camera_y -= scroll_speed;
    }
    if(al_key_down(&g_kstate, ALLEGRO_KEY_DOWN))
    {
        the_level->camera_y += scroll_speed;
    }
    if(al_key_down(&g_kstate, ALLEGRO_KEY_LEFT))
    {
        the_level->camera_x -= scroll_speed;
    }
    if(al_key_down(&g_kstate, ALLEGRO_KEY_RIGHT))
    {
        the_level->camera_x += scroll_speed;
    }
    if(al_key_down(&g_kstate, ALLEGRO_KEY_T))
    {
        the_level->map = create_level_map(the_level);
        g_location = LC_LEVEL;
        the_player.x = (the_level->start_pos[0].x+0.5) * BLOCK_W;
        the_player.y = (the_level->start_pos[0].y+0.5) * BLOCK_H;

    }
}

void render_and_handle_top_button(int x, int y, Gl_texture* tex, int id)
{
    if(is_between(g_mouse_x, g_mouse_y, x, y, x+64, y+64))
    {
        gtex_render_texture_def_size(tex, x, y-3);
        if(is_between(g_clicked_x, g_clicked_y, x, y, x+64, y+64))
        {
            menu_shown = false;
            switch(id)
            {
                case 0: section = LS_BLOCKS; break;
                case 1: section = LS_ART; break;
                case 2: section = LS_SETTINGS; break;
                case 3: menu_shown = true; break;
            }
        }
    }
    else
    {
        gtex_render_texture_def_size(tex, x, y);
    }
}

void render_lvl_edit_gui()
{
    static int current_page = 1;
    init_gui_rendering();
    render_color_rect_with_frame(left_bar_x, left_bar_y, left_bar_w, DEF_WIN_H, 0xacc6e3, 0x3366cc, 1);
    gtex_render_texture_wh(get_block_texture_by_id(held_block), 10+left_bar_x, 10+left_bar_y, 60, 60);

    render_color_rect_with_frame(top_bar_x, top_bar_y, top_bar_w, top_bar_h, 0xacc6e3, 0x3366cc, 1);
    render_and_handle_top_button(top_bar_x+5+0*76, top_bar_y+5, tex_button_lvl_blocks, 0);
    render_and_handle_top_button(top_bar_x+5+1*76, top_bar_y+5, tex_button_lvl_art, 1);
    render_and_handle_top_button(top_bar_x+5+2*76, top_bar_y+5, tex_button_lvl_settings, 2);
    render_and_handle_top_button(top_bar_x+5+3*76, top_bar_y+5, tex_button_lvl_menu, 3);
    //render_and_handle_top_button(top_bar_x+5+4*76, top_bar_y+5, tex_button_lvl_test, 4);

    if(menu_shown)
    {
        render_and_handle_menu(menu_lvl_edit, menu_x, menu_y);
    }

    if(opened_win == LVLW_LOAD)
    {
        render_color_rect(0, 0, DEF_GUI_W, DEF_GUI_H, 1, 1, 1, 0.5);
        render_color_rect_with_frame(win_x, win_y, g_win_w, g_win_h, 0xacc6e3, 0x3366cc, 1);
        print_text("load level", 0x071E6B, win_x+10, win_y+10, 30, 0);
        int pos;
        if(render_and_handle_button("load", win_x+100, win_y2 - 60, 0, &pos))
        {
            char* name = ptrl_get_pointer(levels_list, selected_lvl);
            char path[64];
            sprintf(path, "levels/%s", name);
            the_level = load_pr2_level(path, false);
            opened_win = LVLW_NONE;
        }
        if(render_and_handle_button("load with art", pos+6, win_y2 - 60, 0, &pos))
        {
            char* name = ptrl_get_pointer(levels_list, selected_lvl);
            char path[64];
            sprintf(path, "levels/%s", name);
            extern ALLEGRO_TIMER* timer_FPS;
            al_stop_timer(timer_FPS);
            the_level = load_pr2_level(path, true);
            al_start_timer(timer_FPS);
            opened_win = LVLW_NONE;
        }
        if(render_and_handle_button("cancel", pos+6, win_y2 - 60, 0, NULL))
        {
            opened_win = LVLW_NONE;
        }
        //pos += 5 + render_and_handle_button("delete", pos, win_y2 - 30, 0, b_delete_level);
        //render_and_handle_button("cancel", pos, win_y2 - 30, 0, b_return_to_editor);

        for(int i=0; i<7; i++)
        {
            char* str = ptrl_get_pointer(levels_list, i + current_page*7-7);
            if(str == NULL) {
                break;
            }
            char name[strlen(str)+1];
            strcpy(name, str);
            name[strlen(str)-4] = 0;
            render_list_elm(name, "description", win_x+10, win_y+60+i*66, g_win_w-20, 62, i+current_page*7-7 == selected_lvl);
            if(is_between(g_clicked_x, g_clicked_y, win_x+10, win_y+60+i*66, win_x+10+g_win_w-20, win_y+60+i*66+62))
            {
                selected_lvl = i + current_page*7-7;
            }
        }

        int new_page = render_and_handle_page_select(win_x+200, win_y+10, current_page, 5);
        if(current_page != new_page)
        {
            current_page = new_page;

        }
    }

    if(al_key_down(&g_kstate, ALLEGRO_KEY_SPACE))
    {
        render_color_rect_with_frame(left_bar_w+left_bar_x+10, left_bar_y, 820, 400, 0xacc6e3, 0x3366cc, 1);
        for(int i=0; i<block_count; i++)
        {
            int x = i % 10;
            int y = i / 10;
            int pos_x = left_bar_w+left_bar_x+10 + x*80 + 10;
            int pos_y = left_bar_y+10 + y*80;
            gtex_render_texture(block_table[i].texture, pos_x, pos_y, pos_x+60, pos_y+60);
            int mx = g_mstate.x;
            int my = g_mstate.y;
            if(mx > pos_x && mx < pos_x+60 && my > pos_y && my < pos_y+60)
            {
                if(g_mstate.buttons == 1)
                {
                    held_block = i;
                }
            }
        }
    }
}

void open_load_lvl_window()
{
    opened_win = LVLW_LOAD;
    menu_shown = false;
    selected_lvl = -1;
    if(levels_list != NULL) ptrl_destroy_list(levels_list);
    levels_list = get_directory_contents("levels");
}

static void b_return_to_editor()
{
    opened_win = LVLW_NONE;
}

static void b_delete_level()
{

}
