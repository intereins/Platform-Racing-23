#pragma once

#include "def.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

char* copy_str(const char* str);
bool is_between(int x, int y, int x1, int y1, int x2, int y2);
void hsv_to_rgb(float h, float s, float v, int *r, int *g, int *b);
int convert_string_to_int(const char *str);
char *load_file(const char *filename);
float map_value_f(float val, float min, float max, float new_min, float new_max);
float random_float();
double2 rotate_point(double2 point, double angle);
float2 rotate_vertex(float2 point, float2 center, float angle);
double num_limit(double param1, double param2, double param3);
double2 data_method_9(number_t param1, number_t param2, number_t param3);
