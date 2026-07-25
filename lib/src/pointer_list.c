#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "pointer_list.h"

Pointer_list* ptrl_create_list(int size, float ext_value)
{
    Pointer_list* ret = malloc(sizeof(Pointer_list));
    ret->data = malloc(size * sizeof(void*));
    ret->elm_destroy = malloc(size * sizeof(bool));
    ret->ext_value = ext_value;
    ret->size = size;
    ret->elm_counter = 0;
    return ret;
}

void ptrl_destroy_list(Pointer_list* list)
{
    for(int i=0; i<list->elm_counter; i++)
    {
        if(list->elm_destroy[i])
        {
            free(list->data[i]);
        }
    }

    free(list->data);
    free(list);
}

void* ptrl_get_pointer(Pointer_list* list, int id)
{
    if(id >= list->elm_counter)
    {
        printf("Error: pointer id too big (%i >= %i)\n", id, list->elm_counter);
        return NULL;
    }
    return list->data[id];
}

void ptrl_add_pointer(Pointer_list* list, void* val, bool destroy)
{
    //extend if necessary
    if(list->elm_counter == list->size)
    {
        int new_size = ceil(list->size*list->ext_value);

        void* new_ptr = realloc(list->data, new_size * sizeof(void*));
        if(new_ptr == NULL)
        {
            printf("Error: could not extend list\n");
            return;
        }
        list->data = new_ptr;

        new_ptr = realloc(list->elm_destroy, new_size * sizeof(bool));
        if(new_ptr == NULL)
        {
            printf("Error: could not extend list\n");
            return;
        }
        list->elm_destroy = new_ptr;
        list->size = new_size;
    }

    //add element
    list->data[list->elm_counter] = val;
    list->elm_destroy[list->elm_counter] = destroy;
    list->elm_counter++;
}

void ptrl_remove_elm(Pointer_list* list, int id)
{
    if(id >= list->elm_counter)
    {
        printf("Error %i >= %i at %s %i\n", id, list->elm_counter, __FILE__, __LINE__);
        return;
    }
    
    list->data[id] = list->data[list->elm_counter-1];
    list->elm_counter--;
}
