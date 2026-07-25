#include <stdlib.h> //273 864
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "expanded_list.h"

Extended_list* exl_create(int list_size, int elm_size, float ext_value)
{
    Extended_list* list = malloc(list_size*sizeof(Extended_list));
    list->data = malloc(list_size*elm_size);
    list->elm_count = 0;
    list->elm_size = elm_size;
    list->list_size = list_size;
    list->ext_value = ext_value;
    return list;
}

void exl_destroy(Extended_list* list)
{
    if(list != NULL)
    {
        free(list->data);
        free(list);
    }
}

void exl_clear(Extended_list* list)
{
    list->elm_count = 0;
}

bool exl_get_elm(Extended_list* list, int id, void* ret)
{
    if(id >= list->list_size)
    {
        printf("Error id: %i, list_size: %i at %s %i\n", id, list->list_size, __FILE__, __LINE__);
        return false;
    }
    memcpy(ret, (char*) list->data + id*list->elm_size, list->elm_size);
    return true;
}

void* exl_get_elm_ptr(Extended_list* list, int id)
{
    if(id >= list->elm_count || id < 0)
    {
        printf("Error id: %i, elm_count: %i, list_size: %i at %s %i\n", id, list->elm_count, list->list_size, __FILE__, __LINE__);
        exit(1);
        return NULL;
    }

    return (char*) list->data + id*list->elm_size;
}

void exl_add_elm(Extended_list* list, void* elm)
{
    //extend if necessary
    if(list->elm_count == list->list_size)
    {
        int new_size = floor(list->list_size*list->ext_value);
        void* new_ptr = realloc(list->data, new_size * list->elm_size);
        if(new_ptr == NULL)
        {
            printf("Error: failed to extend list to %i\n", new_size);
            return;
        }
        list->data = new_ptr;
        list->list_size = new_size;
    }

    memcpy((char*) list->data + list->elm_count * list->elm_size, elm, list->elm_size);
    list->elm_count++;
}

void* exl_add_get_ptr(Extended_list* list)
{
    //extend if necessary
    if(list->elm_count == list->list_size)
    {
        int new_size = floor(list->list_size*list->ext_value);
        void* new_ptr = realloc(list->data, new_size * list->elm_size);
        if(new_ptr == NULL)
        {
            printf("Error: failed to extend list to %i\n", new_size);
            return NULL;
        }
        list->data = new_ptr;
        list->list_size = new_size;
    }

    list->elm_count++;
    return (char*)list->data + (list->elm_count-1) * list->elm_size;
}

void exl_remove_elm(Extended_list* list, int id)
{
    if(id >= list->elm_count)
    {
        printf("Error %i >= %i at %s %i\n", id, list->elm_count, __FILE__, __LINE__);
        return;
    }

    memcpy((char*) list->data + id*list->elm_size, (char*) list->data + (list->elm_count-1) * list->elm_size, list->elm_size);
    list->elm_count--;
}
