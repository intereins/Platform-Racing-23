#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include "system_funcs.h"

Pointer_list* get_directory_contents(const char* path)
{
	Pointer_list* list = ptrl_create_list(20, 1.5);
	DIR* d;
	struct dirent* dir;
	d = opendir(path);
	if(d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if(dir->d_name[0] == '.') continue;

			char* string = calloc(strlen(dir->d_name)+1, 1);
			strcpy(string, dir->d_name);
			ptrl_add_pointer(list, string, false);
		}
		closedir(d);
	}
	return list;
}

void create_directory(const char* dir)
{
    //mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

bool dir_exists(const char* dir)
{
    /*struct stat st = {0};
    return stat(dir, &st) != -1;*/
}

clock_t get_time_s()
{
    /*static struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec;*/
}

clock_t get_time_ms()
{
	return clock();
    /*static struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec*1000 + t.tv_usec/1000;*/
}

clock_t get_time_ds()
{
    /*static struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec*10 + t.tv_usec/100000;*/
}
