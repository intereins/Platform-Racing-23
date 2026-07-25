#include <allegro5/allegro.h>
#include <stdbool.h>
#include "control2.h"
#include "tools.h"

ALLEGRO_MOUSE_STATE g_mstate;
ALLEGRO_KEYBOARD_STATE g_kstate;
ALLEGRO_EVENT_QUEUE* event_queue_txt;
int g_mouse_x;
int g_mouse_y;
int g_clicked_x;
int g_clicked_y;

void enable_txt_queue()
{
    al_register_event_source(event_queue_txt, al_get_keyboard_event_source());
}

void disable_txt_queue()
{
    al_unregister_event_source(event_queue_txt, al_get_keyboard_event_source());
}

bool is_mouse_between_wh(int x, int y, int w, int h)
{
    return g_mouse_x >= x && g_mouse_x <= x+w && g_mouse_y >= y && g_mouse_y <= y+h;
}

bool clicked_between(int x1, int y1, int x2, int y2)
{
    return is_between(g_clicked_x, g_clicked_y, x1, y1, x2, y2);
}

bool clicked_between_wh(int x, int y, int w, int h)
{
    return is_between(g_clicked_x, g_clicked_y, x, y, x+w, y+h);
}

void handle_control()
{
    static bool lock;
    al_get_mouse_state(&g_mstate);
    al_get_keyboard_state(&g_kstate);
    g_mouse_x = g_mstate.x;
    g_mouse_y = g_mstate.y;
    g_clicked_x = -1;
    g_clicked_y = -1;

    if(g_mstate.buttons == 1 && lock)
    {
        lock = false;
        g_clicked_x = g_mouse_x;
        g_clicked_y = g_mouse_y;
    }
    if(g_mstate.buttons == 0)
    {
        lock = true;
        g_clicked_x = -1;
        g_clicked_y = -1;
    }
}