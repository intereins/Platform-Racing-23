#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "tools.h"
#include "def.h"
#include "control2.h"

bool is_between(int x, int y, int x1, int y1, int x2, int y2)
{
    return x >= x1 && x <= x2 && y >= y1 && y <= y2;
}

void hsv_to_rgb(float h, float s, float v, int* r, int* g, int* b)
{
    if (h < 0) h += 360;
    if (h >= 360) h -= 360;

    float c = v * s;
    float x = c * (1.0f - (float)(fabs(fmod(h / 60.0f, 2.0f) - 1.0f)));
    float m = v - c;

    float r1 = 0, g1 = 0, b1 = 0;

    if (h >= 0 && h < 60) { r1 = c; g1 = x; b1 = 0; }
    else if (h >= 60 && h < 120) { r1 = x; g1 = c; b1 = 0; }
    else if (h >= 120 && h < 180) { r1 = 0; g1 = c; b1 = x; }
    else if (h >= 180 && h < 240) { r1 = 0; g1 = x; b1 = c; }
    else if (h >= 240 && h < 300) { r1 = x; g1 = 0; b1 = c; }
    else if (h >= 300 && h < 360) { r1 = c; g1 = 0; b1 = x; }

    *r = (int)((r1 + m) * 255.0f);
    *g = (int)((g1 + m) * 255.0f);
    *b = (int)((b1 + m) * 255.0f);
}

int convert_string_to_int(const char* str)
{
    int ret;
    sscanf(str, "%i", &ret);
    return ret;
}

char* load_file(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* ret = malloc(len);
    fread(ret, 1, len, file);
    fclose(file);
    return ret;
}

float map_value_f(float val, float min, float max, float new_min, float new_max)
{
    return (val-min) / (max-min) * (new_max-new_min) + new_min;
}

float random_float()
{
    return rand() / (float)RAND_MAX;
}

double2 rotate_point(double2 point, double angle)
{
    return (double2) {
        .x = point.x*cos(angle) - point.y*sin(angle),
        .y = point.x*sin(angle) + point.y*cos(angle)
    };
}

float2 rotate_vertex(float2 point, float2 center, float angle)
{
    point.x -= center.x;
    point.y -= center.y;

    return (float2) {
        .x = point.x*cos(angle) - point.y*sin(angle) + center.x,
        .y = point.x*sin(angle) + point.y*cos(angle) + center.y
    };
}

double num_limit(double param1, double param2, double param3)
{
    if(param1 > param3)
    {
        param1 = param3;
    }
    else if(param1 < param2)
    {
        param1 = param2;
    }
    return param1;
}

double2 data_method_9(number_t param1, number_t param2, number_t param3)
{
    int _loc4_ = param1;
    int _loc5_ = param2;
    if(param3 > 180)
    {
        param3 = -360 + param3;
    }
    else if(param3 < -180)
    {
        param3 = 360 + param3;
    }
    if(param3 == 90)
    {
        _loc4_ = param2;
        _loc5_ = -param1;
    }
    else if(fabs(param3) == 180)
    {
        _loc4_ = -param1;
        _loc5_ = -param2;
    }
    else if(param3 == -90)
    {
        _loc4_ = -param2;
        _loc5_ = param1;
    }
    return (double2) {.x = _loc4_, .y = _loc5_};
}
