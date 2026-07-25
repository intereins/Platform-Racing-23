#pragma once

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdarg.h>
#include "gl_texture.h"
#include "pointer_list.h"
#include "objects.h"

#define BUTTON_H 40

typedef struct Label
{
    const char* str;
    int color;
    float x;
    float y;
} Label;

typedef struct Menu_item
{
    char* name;
    void (*func_ptr)();
} Menu_item;

typedef struct Menu
{
    struct Menu_item* items;
    int item_count;
} Menu;

typedef struct Text_input
{
    char content[64];
    int x;
    int y;
    short width;
    byte str_len;
    byte cursor_pos;
    bool active;
} Text_input;

typedef struct Slider
{
    int x;
    int y;
    short length;
    short pos;
    short mouse_y;
    bool sliding;
} Slider;

typedef struct Gui_list
{
    Slider slider;
    char** items;
    int x;
    int y;
    short width;
    short height;
    short item_count;
    short selected;
    short add_counter;
    bool opened;
    bool direction;
} Gui_list;

typedef struct Page_select
{
    int x;
    int y;
    byte current_page;
} Page_select;

extern Gl_texture* tex_block_test;
extern Gl_texture* tex_block_frozen;
extern Gl_texture* tex_block_not_found;
extern Gl_texture* tex_hat_propeller;
extern Gl_texture* tex_hat_santa[4];
extern Gl_texture* tex_hat_moon[3];
extern Gl_texture* tex_hat_item[3];
extern Gl_texture* tex_hat_top;
extern Gl_texture* tex_hat_cheese;
extern Gl_texture* tex_hat_mhelmet;
extern Gl_texture* tex_hat_nurse;

extern Gl_texture* tex_button;
extern Gl_texture* tex_button_hover;

extern Gl_texture* tex_list_elm;
extern Gl_texture* tex_list_elm_select;

extern Gl_texture* tex_input_list_arrow;
extern Gl_texture* tex_slider;
extern Gl_texture* tex_slider_up;
extern Gl_texture* tex_slider_down;

extern Gl_texture* tex_button_lvl_blocks;
extern Gl_texture* tex_button_lvl_art;
extern Gl_texture* tex_button_lvl_settings;
extern Gl_texture* tex_button_lvl_menu;
extern Gl_texture* tex_button_lvl_test;

extern ALLEGRO_FONT* font_pr3[50];

extern Gui_list* gui_list_music;

void init_gui();
void init_font();
void load_textures();
void print_text(const char *str, int color, float x, float y, int size, int flags);
bool render_and_handle_button(const char *str, int x, int y, int w, int *ret_end_pos);
int render_and_handle_page_select(int x, int y, int page, int page_count);
Menu *create_menu(int item_count, ...);
void render_and_handle_menu(Menu *menu, int x, int y);
void render_list_elm(const char *title, const char *desc, int x, int y, int w, int h, byte state);
void render_and_handle_text_input(Text_input *input);
void render_and_handle_gui_list(Gui_list *list);

Gui_list *create_gui_list(int x, int y, int w, int item_count, ...);
