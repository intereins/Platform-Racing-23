#ifndef MAP_H
#define MAP_H

#include "level.h"

struct Player;

void render_level_map(Level *lvl, struct Player *pl);
Gl_texture* create_level_map(Level* lvl);

#endif // MAP_H
