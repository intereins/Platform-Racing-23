#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <time.h>
//#include <GLES2/gl2.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include "gl_texture.h"
#include "render.h"
#include "def.h"
#include "graph.h"
#include "level.h"
#include "player.h"
#include "main.h"
#include "level_editor.h"
#include "block_piece.h"
#include "sounds.h"
#include "items.h"
#include "debug.h"
#include "internet.h"
#include "animation.h"
#include "objects.h"
#include "replay.h"
#include "control2.h"

int g_win_w=1100, g_win_h=800;
bool done;
ALLEGRO_DISPLAY* display;
enum Location g_location = LC_LVL_EDITOR;
ALLEGRO_TIMER* timer_FPS;
int frame_nr = 0;

int main()
{
    if(!al_init())
    {
        al_show_native_message_box(NULL,NULL,NULL,"Allegro couldnt initialize",NULL,0);
    }

    al_set_new_display_flags(ALLEGRO_OPENGL);
    display = al_create_display(g_win_w,g_win_h);

    if(!display)
    {
        al_show_native_message_box(NULL,NULL,NULL,"Couldnt create Screen",NULL,0);
    }

    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_install_mouse();
    al_install_audio();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_acodec_addon();

    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    ALLEGRO_EVENT_QUEUE* event_queue_timer = al_create_event_queue();
    event_queue_txt = al_create_event_queue();
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));

    timer_FPS = al_create_timer(1.0/54.0);
    al_register_event_source( event_queue_timer, al_get_timer_event_source( timer_FPS ) );
    al_start_timer( timer_FPS );

    init_block_pieces();
    init_audio();
    load_textures();
    load_sounds();
    load_blocks("blocks/blocks.json");
    init_items();
    init_font();
    init_animations();
    init_objects();
    init_level_editor();
    init_rendering_to_texture();
    init_gui();
    srand(time(NULL));

    player_init(&the_player);
    enter_level_editor();

    bool even_frame = true;
    bool double_fps = true;
    number_t prev_cam_x = 0;
    number_t prev_cam_y = 0;
    static bool debug_lock = true;

    while(!done)
    {
        ALLEGRO_EVENT event_timer;
        al_wait_for_event(event_queue_timer, &event_timer);
        even_frame = !even_frame;
        handle_control();

        if(al_key_down(&g_kstate, ALLEGRO_KEY_ESCAPE))
        {
            done = true;
        }

        init_rendering();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(g_location == LC_LEVEL)
        {
            if(even_frame |! double_fps)
            {
                the_level->camera_x = the_player.x-270;
                the_level->camera_y = the_player.y-210;

                render_level(the_level, -the_level->camera_x, -the_level->camera_y);
                render_placed_mines(the_level->camera_x, the_level->camera_y);
                prev_cam_x = the_level->camera_x;
                prev_cam_y = the_level->camera_y;
                render_player(&the_player, 270, 189);
                player_update(&the_player);
                update_level(the_level);
                handle_placed_mines();
                g_cam_x = the_level->camera_x;
                g_cam_y = the_level->camera_y;
                render_objects();
                frame_nr++;
            }
            else
            {
                the_level->camera_x = the_player.x-270;
                the_level->camera_y = the_player.y-210;
                number_t cam_x = (the_level->camera_x + prev_cam_x) / 2.0;
                number_t cam_y = (the_level->camera_y + prev_cam_y) / 2.0;
                render_level(the_level, -cam_x, -cam_y);
                render_placed_mines(cam_x, cam_y);
                render_player(&the_player, 270, 189);
                g_cam_x = cam_x;
                g_cam_y = cam_y;
                render_objects();
            }
            render_item_gui(&the_player);
            render_block_pieces();
            update_block_pieces();
            render_and_handle_anims();
            update_objects();
            update_block_offsets();
            if(!double_fps)
            {
                update_block_pieces();
            }
            render_and_handle_level_gui();
        }
        else if(g_location == LC_LVL_EDITOR)
        {
            render_level_editor();
            lvl_editor_update();
        }

        if(al_key_down(&g_kstate, ALLEGRO_KEY_H) && debug_lock)
        {
            static int hat = 0;
            hat = (hat+1) % 8;
            the_player.hats = 1 << hat;
            debug_lock = false;
        }
        if(!al_key_down(&g_kstate, ALLEGRO_KEY_H)) debug_lock = true;

        do_tests_loop();
        al_flip_display();
    }
    al_destroy_display(display);
    al_destroy_timer(timer_FPS);
    al_destroy_event_queue(event_queue);
    al_flip_display();

    return 0;
}
