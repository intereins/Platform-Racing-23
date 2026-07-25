#pragma once

#include <stdbool.h>

#define rad(d) ((d)*3.14159265/180.0)
#define deg(r) ((r)*180.0/3.14159265)

#define debugPrint printf

#define DEF_WIN_W 550
#define DEF_WIN_H 400
#define DEF_GUI_W 1100
#define DEF_GUI_H 800

typedef unsigned int uint;
typedef double number_t;
typedef unsigned char byte;
typedef signed char ibyte;
typedef unsigned short ushort;

enum Rotation
{
    ROT_0,
    ROT_90,
    ROT_180,
    ROT_270
};

typedef struct byte3
{
    byte x;
    byte y;
    byte z;
} byte3;

typedef struct int2
{
    int x;
    int y;
} int2;

typedef struct float2
{
    float x;
    float y;
} float2;

typedef struct double2
{
    double x;
    double y;
} double2;
