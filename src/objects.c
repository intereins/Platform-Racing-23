#include <stdlib.h>
#include "objects.h"

Pointer_list* obj_list;

void init_objects()
{
    obj_list = ptrl_create_list(10, 2);
}

void add_obj(void* obj)
{
    ptrl_add_pointer(obj_list, obj, true);
}

void update_objects()
{
    for(int i=0; i<obj_list->elm_counter; i++)
    {
        void* obj = ptrl_get_pointer(obj_list, i);
        Obj_funcs* fncs = *((Obj_funcs**)obj);
        Object* object = obj;

        if(object->active && fncs != NULL && fncs->update != NULL)
        {
            fncs->update(obj);
            if(object->dead)
            {
                ptrl_remove_elm(obj_list, i);
                i--;
            }
        }
    }
}

void render_objects()
{
    for(int i=0; i<obj_list->elm_counter; i++)
    {
        void* obj = ptrl_get_pointer(obj_list, i);
        Obj_funcs* fncs = *((Obj_funcs**)obj);
        if(fncs != NULL && fncs->render != NULL)
        {
            fncs->render(obj);
        }
    }
}
