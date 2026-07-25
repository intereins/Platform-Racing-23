#ifndef POINTER_LIST_H
#define POINTER_LIST_H

#include <stdbool.h>

typedef struct Pointer_list Pointer_list;

struct Pointer_list
{
    void** data;
    bool* elm_destroy;
    int size;
    int elm_counter;
    float ext_value;
};

Pointer_list* ptrl_create_list(int size, float ext_value);
void* ptrl_get_pointer(Pointer_list* list, int id);
void ptrl_add_pointer(Pointer_list* list, void* val, bool destroy);
void ptrl_remove_elm(Pointer_list* list, int id);
void ptrl_destroy_list(Pointer_list* list);

#endif // POINTER_LIST_H
