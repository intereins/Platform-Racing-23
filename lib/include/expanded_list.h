#ifndef EXPANDED_LIST_H
#define EXPANDED_LIST_H

#include <stdbool.h>

typedef struct Extended_list Extended_list;

struct Extended_list
{
    void* data;
    int list_size;
    int elm_size;
    int elm_count;
    float ext_value;
};

Extended_list* exl_create(int list_size, int elm_size, float ext_value);
void exl_destroy(Extended_list* list);
void exl_clear(Extended_list* list);
bool exl_get_elm(Extended_list* list, int id, void* ret);
void exl_add_elm(Extended_list* list, void* elm);
void* exl_add_get_ptr(Extended_list* list);
void exl_remove_elm(Extended_list* list, int id);
void* exl_get_elm_ptr(Extended_list* list, int id);

#endif // EXPANDED_LIST_H
