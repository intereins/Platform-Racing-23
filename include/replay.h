#pragma once

#include "expanded_list.h"
#include "def.h"

extern Extended_list* bound_replay;
extern byte g_replay_mode;

void replay_add_frame(Extended_list *replay, bool up, bool down, bool left, bool right, bool space);
