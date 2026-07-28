#include <stdio.h>
#include "graph.h"
#include "player.h"
#include "main.h"
#include "tools.h"
#include "render.h"
#include "control2.h"
#include "debug.h"

Gl_texture* tex_block_test = NULL;
Gl_texture* tex_block_frozen = NULL;
Gl_texture* tex_block_not_found = NULL;
Gl_texture* tex_hat_propeller = NULL;
Gl_texture* tex_hat_santa[4];
Gl_texture* tex_hat_moon[3];
Gl_texture* tex_hat_item[3];
Gl_texture* tex_hat_top = NULL;
Gl_texture* tex_hat_cheese = NULL;
Gl_texture* tex_hat_mhelmet = NULL;
Gl_texture* tex_hat_nurse = NULL;

Gl_texture* tex_button;
Gl_texture* tex_button_hover;

Gl_texture* tex_list_elm;
Gl_texture* tex_list_elm_select;

Gl_texture* tex_input_list_arrow;
Gl_texture* tex_slider;
Gl_texture* tex_slider_up;
Gl_texture* tex_slider_down;

Gl_texture* tex_button_lvl_blocks;
Gl_texture* tex_button_lvl_art;
Gl_texture* tex_button_lvl_settings;
Gl_texture* tex_button_lvl_menu;
Gl_texture* tex_button_lvl_test;

Gl_texture* tex_page_arrows;

Pointer_list* label_list;

ALLEGRO_FONT* font_pr2[50];
ALLEGRO_FONT* font_pr3[50];

Gui_list* gui_list_music;

void init_gui()
{
    gui_list_music = create_gui_list((DEF_GUI_W-350)/2, DEF_GUI_H-50, 350, 20, "none");
}

void init_font()
{
    font_pr3[15] = al_load_font("assets/font/Action Man.ttf", 15, 0);
    font_pr3[20] = al_load_font("assets/font/Action Man.ttf", 20, 0);
    font_pr3[26] = al_load_font("assets/font/Action Man.ttf", 26, 0);
    font_pr3[28] = al_load_font("assets/font/Action Man.ttf", 28, 0);
    font_pr3[30] = al_load_font("assets/font/Action Man.ttf", 30, 0);
    font_pr2[26] = al_load_font("assets/font/Verdana.ttf", 26, 0);
    font_pr2[30] = al_load_font("assets/font/Verdana.ttf", 30, 0);

    label_list = ptrl_create_list(10, 2);
}

void init_hats()
{
	tex_hat_propeller = gtex_load_texture("assets/img/hats/propeller.png");
	tex_hat_santa[0] = gtex_load_texture("assets/img/hats/santa_base.png");
	tex_hat_santa[1] = gtex_load_texture("assets/img/hats/santa_part1.png");
	tex_hat_santa[2] = gtex_load_texture("assets/img/hats/santa_part2.png");
	tex_hat_santa[3] = gtex_load_texture("assets/img/hats/santa_shade.png");
	tex_hat_moon[0] = gtex_load_texture("assets/img/hats/moon_base.png");
	tex_hat_moon[1] = gtex_load_texture("assets/img/hats/moon_shade.png");
	tex_hat_moon[2] = gtex_load_texture("assets/img/hats/moon_part1.png");
	tex_hat_item[0] = gtex_load_texture("assets/img/hats/item_base.png");
	tex_hat_item[1] = gtex_load_texture("assets/img/hats/item_shade.png");
	tex_hat_item[2] = gtex_load_texture("assets/img/hats/item_part1.png");
	tex_hat_top = gtex_load_texture("assets/img/hats/top.png");
	tex_hat_cheese = gtex_load_texture("assets/img/hats/cheese.png");
	tex_hat_mhelmet = gtex_load_texture("assets/img/hats/mhelmet.png");
	tex_hat_nurse = gtex_load_texture("assets/img/hats/nurse.png");
}

void load_textures()
{
    init_hats();
    tex_block_frozen = gtex_load_texture("blocks/frozen.png");
    tex_block_not_found = gtex_load_texture("assets/img/blocks/classic/not found.png");

    tex_button = gtex_load_texture("assets/img/gui/button.png");
    tex_button_hover = gtex_load_texture("assets/img/gui/button_hover.png");

    tex_list_elm = gtex_load_texture("assets/img/gui/list_elm.png");
    tex_list_elm_select = gtex_load_texture("assets/img/gui/list_elm_select.png");

    tex_input_list_arrow = gtex_load_texture("assets/img/gui/input_list_arrow.png");
    tex_slider = gtex_load_texture("assets/img/gui/slider.png");
    tex_slider_up = gtex_load_texture("assets/img/gui/arrow_up.png");
    tex_slider_down = gtex_load_texture("assets/img/gui/arrow_down.png");

    tex_button_lvl_blocks = gtex_load_texture("assets/img/gui/lvl_blocks.png");
    tex_button_lvl_art = gtex_load_texture("assets/img/gui/lvl_art.png");
    tex_button_lvl_settings = gtex_load_texture("assets/img/gui/lvl_settings.png");
    tex_button_lvl_menu = gtex_load_texture("assets/img/gui/lvl_menu.png");
    tex_button_lvl_test = gtex_load_texture("assets/img/gui/lvl_test.png");

    tex_page_arrows = gtex_load_texture("assets/img/gui/page_arrows.png");
}

void print_text(const char* str, int color, float x, float y, int size, int flags)
{
    al_draw_text(font_pr3[size], al_map_rgb(color >> 16, (color >> 8) & 255, color & 255), x, y, flags, str);
}

bool render_and_handle_button(const char* str, int x, int y, int w, int* ret_end_pos)
{
    int button_w = w;
    bool ret = false;
    if(w == 0)
    {
        button_w = al_get_text_width(font_pr3[30], str) + 6;
    }
    if(is_between(g_mouse_x, g_mouse_y, x, y, x+button_w, y+tex_button->img->height))
    {
        gtex_render_texture_frag_def_size(tex_button, x, y, 0, 0, 8.0, BUTTON_H);
        gtex_render_texture_frag_def_size(tex_button, x+button_w, y, 9, 0, 17, BUTTON_H);
        gtex_render_texture_frag(tex_button, x+8, y, x+button_w, y+BUTTON_H, 8/17.0, 0, 9/17.0, 1);

        /*gtex_render_texture_frag_def_size(tex_button_hover, x, y, 0, 0, 4.0, BUTTON_H);
        gtex_render_texture_frag_def_size(tex_button_hover, x+button_w, y, 5, 0, 9, BUTTON_H);
        gtex_render_texture_frag(tex_button_hover, x+4, y, x+button_w, y+BUTTON_H, 4/9.0, 0, 5/9.0, 1);*/
        if(is_between(g_clicked_x, g_clicked_y, x, y, x+button_w, y+tex_button->img->height))
        {
            ret = true;
        }
    }
    else
    {
        gtex_render_texture_frag_def_size(tex_button, x, y, 0, 0, 8.0, BUTTON_H);
        gtex_render_texture_frag_def_size(tex_button, x+button_w, y, 9, 0, 17, BUTTON_H);
        gtex_render_texture_frag(tex_button, x+8, y, x+button_w, y+BUTTON_H, 8/17.0, 0, 9/17.0, 1);
    }

    print_text(str, 0, x+button_w/2+3, y+10, 28, ALLEGRO_ALIGN_CENTER);
    if(ret_end_pos != NULL) {
        *ret_end_pos = x+button_w+6;
    }
    return ret;
}

int render_and_handle_page_select(int x, int y, int page, int page_count)
{
    if(is_mouse_between_wh(x, y, 49, 49)) {
        gtex_render_texture_frag_def_size_invh(tex_page_arrows, x, y, 49, 0, 98, 49);
    } else {
        gtex_render_texture_frag_def_size_invh(tex_page_arrows, x, y, 0, 0, 49, 49);
    }

    if(is_mouse_between_wh(x+57+4*52, y, 49, 49)) {
        gtex_render_texture_frag_def_size(tex_page_arrows, x+57+4*52, y, 49, 0, 98, 49);
    } else {
        gtex_render_texture_frag_def_size(tex_page_arrows, x+57+4*52, y, 0, 0, 49, 49);
    }

    render_color_rect_with_frame(x+57+0*52, y, 44, 44, 0xA1C7F4, 0x071E6B, 2);
    render_color_rect_with_frame(x+57+1*52, y, 44, 44, 0xA1C7F4, 0x071E6B, 2);
    render_color_rect_with_frame(x+57+2*52, y, 44, 44, 0xA1C7F4, 0x071E6B, 2);
    render_color_rect_with_frame(x+57+3*52, y, 44, 44, 0xA1C7F4, 0x071E6B, 2);


    char page_nr_str[4];
    sprintf(page_nr_str, "%i", page);
    print_text(page_nr_str, 0x071E6B, x+57+0*52+6, y+13, 30, 0);
    sprintf(page_nr_str, "%i", page+1);
    print_text(page_nr_str, 0x071E6B, x+57+1*52+6, y+13, 30, 0);
    sprintf(page_nr_str, "%i", page+2);
    print_text(page_nr_str, 0x071E6B, x+57+2*52+6, y+13, 30, 0);
    print_text("...", 0x071E6B, x+57+3*52+6, y+13, 30, 0);

    if(clicked_between_wh(x, y, 49, 49))
    {
        return page > 1 ? page-1 : 1;
    }
    if(clicked_between_wh(x+57+4*52, y, 49, 49))
    {
        return page+1;
    }
    return page;
}

Menu* create_menu(int item_count, ...)
{
    va_list args;
    va_start(args, item_count);
    Menu* ret = malloc(sizeof(Menu));
    ret->items = malloc(item_count*sizeof(Menu_item));
    ret->item_count = item_count;
    for(int i=0; i<item_count; i++)
    {
        ret->items[i].name = va_arg(args, void*);
        ret->items[i].func_ptr = va_arg(args, void*);
    }
    va_end(args);
    return ret;
}

void render_and_handle_menu(Menu* menu, int x, int y)
{
    int menu_w = 222;
    int menu_h = menu->item_count*30;
    render_color_rect_with_frame(x, y, menu_w, menu_h, 0xacc6e3, 0x3366cc, 1);
    int font_size = 26;
    int selected = -1;
    if(is_between(g_mouse_x, g_mouse_y, x, y, x+menu_w-1, y+menu_h-1))
    {
        selected = ((uint)g_mouse_y - (uint)y) / 30;
    }
    if(selected >= 0 && selected < menu->item_count)
    {
        render_color_rect_wh(x+4, y + selected*30, 214, 26, 135 << 16 | 174 << 8 | 211);
        if(is_between(g_clicked_x, g_clicked_y, x, y+selected*30 + 2, x+107, y+selected*30 + 28))
        {
            if(menu->items[selected].func_ptr != NULL)
            {
                menu->items[selected].func_ptr();
            }
        }
    }
    for(int i=0; i<menu->item_count; i++)
    {
        print_text(menu->items[i].name, 0x071E6B, x+6, y+4+i*30, font_size, 0);
    }
}

void render_list_elm(const char* title, const char* desc, int x, int y, int w, int h, byte state)
{
    switch(state)
    {
        case 0: gtex_render_texture(tex_list_elm, x, y, x+w, y+h); break;
        case 1: gtex_render_texture(tex_list_elm_select, x, y, x+w, y+h); break;
        case 2: gtex_render_texture(tex_list_elm, x, y, x+w, y+h); break;
    }
    print_text(title, 0x071E6B, x+4, y+8, 28, 0);
    print_text(desc, 0x071E6B, x+4, y+40, 20, 0);
}

void render_and_handle_text_input(Text_input* input)
{
    render_color_rect_wh(input->x, input->y, input->width, 42, 0x071E6B);
    render_color_rect_wh(input->x+2, input->y+2, input->width-4, 38, 0x83B6F4);
    render_color_rect_wh(input->x+8, input->y+8, input->width-16, 26, 0x025261);
    render_color_rect_wh(input->x+10, input->y+10, input->width-20, 22, input->active ? 0x7ACCDB : 0x39B3CA);

    print_text(input->content, 0x071E6B, input->x+11, input->y+12, 26, 0);
    if(input->active)
    {
        int cursor_x = al_get_text_width(font_pr3[26], input->content);
        glDisable(GL_TEXTURE_2D);
        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
        glVertex2f(input->x + cursor_x + 12, input->y + 11);
        glVertex2f(input->x + cursor_x + 12, input->y + 32);
        glEnd();
        glColor3f(1, 1, 1);
        glEnable(GL_TEXTURE_2D);
        ALLEGRO_EVENT event;
        al_get_next_event(event_queue_txt, &event);

        if(event.type == ALLEGRO_EVENT_KEY_CHAR)
        {
            int key = event.keyboard.keycode;
            if(key == ALLEGRO_KEY_BACKSPACE && input->str_len > 0) {
                input->str_len--;
                input->content[input->str_len] = 0;
            }
            else if(key == ALLEGRO_KEY_SPACE) {
                input->content[input->str_len] = ' ';
                input->str_len++;
            }
            else if(event.keyboard.unichar != 0) {
                input->content[input->str_len] = event.keyboard.unichar;
                input->str_len++;
            }
        }

    }

    if(is_between(g_clicked_x, g_clicked_y, input->x, input->y, input->x+input->width, input->y+42))
    {
        input->active = true;
        enable_txt_queue();
    }
    else if(g_clicked_x != -1)
    {
        printf("clicked %i %i\n", g_clicked_x, g_clicked_y);
        input->active = false;
        disable_txt_queue();
    }
}

void render_and_handle_slider(Slider* slider)
{
    render_color_rect_with_frame(slider->x, slider->y+26, 30, slider->length-52, 0xA1C7F4, 0x5977AE, 1);
    gtex_render_texture_def_size(tex_slider_up, slider->x, slider->y);
    gtex_render_texture_def_size(tex_slider_down, slider->x, slider->y+slider->length-30);
    gtex_render_texture_def_size(tex_slider, slider->x, slider->y+30 + slider->pos);

    if(clicked_between_wh(slider->x, slider->y+30, 60, 162))
    {
        slider->sliding = true;
        slider->mouse_y = g_mouse_y - slider->pos;
    }
    if(g_mstate.buttons == 0)
    {
        slider->sliding = false;
    }
    if(slider->sliding)
    {
        slider->pos = g_mouse_y - slider->mouse_y;
        if(slider->pos < 0) {
            slider->pos = 0;
        }
        else if(slider->pos > slider->length - 141) {
            slider->pos = slider->length - 141;
        }
    }
}

void render_and_handle_gui_list(Gui_list* list)
{
    if(list->opened)
    {
        if(list->direction)
        {
            render_color_rect_with_frame(list->x, list->y-list->height, list->width, list->height, 0xacc6e3, 0x3366cc, 1);
            glBindTexture(GL_TEXTURE_2D, tex_fb);
            glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, list->x, list->y+list->height-13 - (list->y-403)*2, 350, 47);
            float slider_frac = list->slider.pos / (list->slider.length - 141.0);
            int val = list->item_count*42 - list->height + 16;
            for(int i=0; i<list->item_count; i++)
            {
                if(list->y+51 + i*42 - slider_frac*val - 206 > list->slider.y && list->y+51 + i*42 - slider_frac*val < list->y+42+list->height)
                {
                    int color = 0x39B3CA;
                    if(is_mouse_between_wh(list->x+9, list->y+51 + i*42 - slider_frac*val - 244, list->width-56, 38))
                    {
                        color = 0x7ACCDB;
                        if(clicked_between_wh(list->x+9, list->y+51 + i*42 - slider_frac*val - 244, list->width-56, 38))
                        {
                            list->selected = i;
                            list->opened = false;
                        }
                    }
                    render_color_rect_wh(list->x+9, list->y+51 + i*42 - slider_frac*val - 244, list->width-56, 38, color);
                    print_text(list->items[i], 0x071E6B, list->x+11, list->y+60 + i*42 - slider_frac*val - 244, 26, 0);
                }
            }
            render_and_handle_slider(&list->slider);
            render_color_rect_wh(list->x+1, list->y+43-52, list->width-2, 8, 0xacc6e3);

            glBindTexture(GL_TEXTURE_2D, tex_fb);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(list->x, list->y-list->height-40+47);
            glTexCoord2f(1, 0); glVertex2f(list->x+list->width, list->y-list->height-40+47);
            glTexCoord2f(1, 1); glVertex2f(list->x+list->width, list->y-list->height-40);
            glTexCoord2f(0, 1); glVertex2f(list->x, list->y-list->height-40);
            glEnd();
        }
        else
        {
            render_color_rect_with_frame(list->x, list->y+42, list->width, list->height, 0xacc6e3, 0x3366cc, 1);
            glBindTexture(GL_TEXTURE_2D, tex_fb);
            glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, list->x, list->y+list->height+42-124-(list->y-200)*2, 350, 47);
            float slider_frac = list->slider.pos / (list->slider.length - 141.0);
            int val = list->item_count*42 - list->height + 16;
            for(int i=0; i<list->item_count; i++)
            {
                if(list->y+51 + i*42 - slider_frac*val + 38 > list->y+51 && list->y+51 + i*42 - slider_frac*val < list->y+42+list->height)
                {
                    int color = 0x39B3CA;
                    if(is_mouse_between_wh(list->x+9, list->y+51 + i*42 - slider_frac*val, list->width-56, 38))
                    {
                        color = 0x7ACCDB;
                        if(clicked_between_wh(list->x+9, list->y+51 + i*42 - slider_frac*val, list->width-56, 38))
                        {
                            list->selected = i;
                            list->opened = false;
                        }
                    }
                    render_color_rect_wh(list->x+9, list->y+51 + i*42 - slider_frac*val, list->width-56, 38, color);
                    print_text(list->items[i], 0x071E6B, list->x+11, list->y+60 + i*42 - slider_frac*val, 26, 0);
                }
            }
            render_and_handle_slider(&list->slider);
            render_color_rect_wh(list->x+1, list->y+43, list->width-2, 8, 0xacc6e3);

            glBindTexture(GL_TEXTURE_2D, tex_fb);
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2f(list->x, list->y+list->height+35+47);
            glTexCoord2f(1, 0); glVertex2f(list->x+list->width, list->y+list->height+35+47);
            glTexCoord2f(1, 1); glVertex2f(list->x+list->width, list->y+list->height+35);
            glTexCoord2f(0, 1); glVertex2f(list->x, list->y+list->height+35);
            glEnd();
        }
    }

    render_color_rect_wh(list->x, list->y, list->width, 42, 0x071E6B);
    render_color_rect_wh(list->x+2, list->y+2, list->width-4, 38, 0x83B6F4);
    render_color_rect_wh(list->x+8, list->y+8, list->width-16, 26, 0x025261);
    render_color_rect_wh(list->x+10, list->y+10, list->width-20, 22, list->opened ? 0x7ACCDB : 0x39B3CA);
    gtex_render_texture_def_size(tex_input_list_arrow, list->x+list->width-24-8, list->y+9);
    print_text(list->items[list->selected], 0x071E6B, list->x+11, list->y+13, 26, 0);

    if(clicked_between(list->x, list->y, list->x+list->width, list->y+42))
    {
        list->opened = true;
    }
}

Gui_list* create_gui_list(int x, int y, int w, int item_count, ...)
{
    Gui_list* ret = calloc(1, sizeof(Gui_list));

    ret->x = x;
    ret->y = y;
    ret->width = w;
    ret->height = 200;
    ret->slider.length = 200-16;
    ret->slider.x = x + w - 39;
    ret->item_count = item_count;
    ret->items = malloc(item_count * sizeof(void*));
    ret->direction = 1;
    ret->slider.y = y - 9 - ret->slider.length;

    va_list args;
    va_start(args, item_count);
    for(int i=0; i<item_count; i++)
    {
        ret->items[i] = va_arg(args, char*);
    }
    va_end(args);
    return ret;
}

void add_item_to_gui_list(Gui_list* list, const char* name)
{
    list->items = realloc(list->items, (list->item_count+1) * sizeof(void*));
    list->items[list->item_count] = copy_str(name);
    list->item_count++;    
}
