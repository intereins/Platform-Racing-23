#include "replay.h"

Extended_list* bound_replay;
byte g_replay_mode;

void replay_add_frame(Extended_list* replay, bool up, bool down, bool left, bool right, bool space)
{
    byte val = 0;
    if(up) val |= 1;
    if(down) val |= 2;
    if(left) val |= 4;
    if(right) val |= 8;
    if(space) val |= 16;
    exl_add_elm(replay, &val);
}

