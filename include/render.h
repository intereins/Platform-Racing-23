#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_opengl.h>
//#include <GLES2/gl2.h>
#include "gl_texture.h"
#include "def.h"

extern float g_cam_x;
extern float g_cam_y;
extern unsigned fbo;
extern unsigned tex_fb;

void init_rendering();

void init_gui_rendering();

void init_rendering_to_texture();

void glVertexf2(float2 vex);

void render_texture_rot(Gl_texture *tex, float x, float y, float ang);

void render_texture_rot_point(Gl_texture *tex, float x, float y, float ang, float center_x, float center_y);

void render_texture_rot_point_scaled(Gl_texture *tex, float x, float y, float ang, float center_x, float center_y, float scale);

void render_texture_frag_rot(Gl_texture *tex, float x1, float y1, float x2, float y2, float x, float y, float ang);

void glVertex2f_rc(float x, float y, float angle);

void render_texture_rot_center(Gl_texture *tex, float x1, float y1, float x2, float y2, float angle, int rot);

void render_texture(Gl_texture *tex, float x, float y, float scale, bool reversed);

void render_texture_fr(Gl_texture *tex, float x, float y, float scale, bool reversed, float rot_center_x, float rot_center_y, float angle);

void render_color_rect(float x1, float y1, float x2, float y2, float r, float g, float b, float a);

void render_color_rect_with_frame(float x1, float y1, float w, float h, int color, int frame_color, float thickness);

void render_color_rect_wh(float x1, float y1, float w, float h, int color);

void render_color_rect_whf(float x1, float y1, float w, float h, float r, float g, float b);
