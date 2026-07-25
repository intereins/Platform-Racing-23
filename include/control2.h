#pragma once

#include <allegro5/allegro.h>

extern ALLEGRO_KEYBOARD_STATE g_kstate;
extern ALLEGRO_MOUSE_STATE g_mstate;
extern ALLEGRO_EVENT_QUEUE* event_queue_txt;
extern int g_mouse_x;
extern int g_mouse_y;
extern int g_clicked_x;
extern int g_clicked_y;

void enable_txt_queue();
void disable_txt_queue();
bool is_mouse_between_wh(int x, int y, int w, int h);
bool clicked_between(int x1, int y1, int x2, int y2);
bool clicked_between_wh(int x, int y, int w, int h);
void handle_control();