#ifndef SYSTEM_FUNCS_H
#define SYSTEM_FUNCS_H

#include <time.h>
#include "pointer_list.h"

Pointer_list* get_directory_contents(const char* path);
void create_directory(const char* dir);
bool dir_exists(const char* dir);
clock_t get_time_s();
clock_t get_time_ms();
clock_t get_time_ds();

#endif // SYSTEM_FUNCS_H
