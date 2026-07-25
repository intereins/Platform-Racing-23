#pragma once

#include "pointer_list.h"

#define FNCS_CREATE(name, on_create, f_update, f_render) Obj_funcs name = {.update = (void*)f_update, .render = (void*)f_render}

typedef struct Obj_funcs
{
    void (*on_create)(void*);
    void (*update)(void*);
    void (*on_delete)(void*);
    void (*render)(void*);
} Obj_funcs;

typedef struct Object
{
    Obj_funcs* fncs;
    bool dead;
    bool active;
} Object;

void init_objects();
void add_obj(void *obj);
void update_objects();
void render_objects();
