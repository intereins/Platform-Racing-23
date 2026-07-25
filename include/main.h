#pragma once

#include <allegro5/allegro.h>

enum Location
{
    LC_LEVEL,
    LC_LVL_EDITOR,    
};

extern enum Location g_location;
extern int frame_nr;