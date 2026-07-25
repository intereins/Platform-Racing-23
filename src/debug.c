#include <allegro5/allegro.h>
#include <stdio.h>
#include "debug.h"
#include "main.h"
#include "control2.h"

float var_x = 0;
float var_y = 0;
float var_scale = 1;

void do_tests_loop()
{
    float speed = 0.5;
    if(al_key_down(&g_kstate, ALLEGRO_KEY_D)) {    
        var_x += speed;
    }
    if(al_key_down(&g_kstate, ALLEGRO_KEY_A)) {    
        var_x -= speed;
    }
    if(al_key_down(&g_kstate, ALLEGRO_KEY_S)) {    
        var_y -= speed;
    }
    if(al_key_down(&g_kstate, ALLEGRO_KEY_W)) {    
        var_y += speed;
    }
    if(al_key_down(&g_kstate, ALLEGRO_KEY_PAD_PLUS)) {    
        var_scale += speed;
    }
    if(al_key_down(&g_kstate, ALLEGRO_KEY_PAD_MINUS)) {    
        var_scale -= speed;
    }
   
    //printf("%f %f\n", var_x, var_y);
}