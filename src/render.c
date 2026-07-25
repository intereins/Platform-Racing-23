#include <allegro5/allegro.h>
#include <allegro5/allegro_opengl.h>
//#include <GLES2/gl2.h>
#include <math.h>
#include "render.h"
#include "def.h"
#include "gl_texture.h"
#include "tools.h"

extern int g_win_w, g_win_h;
float g_cam_x;
float g_cam_y;
unsigned fbo;
unsigned tex_fb;

void init_rendering()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glViewport(0, 0, g_win_w, g_win_h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, DEF_WIN_W, DEF_WIN_H, 0, -20, 20);

	glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
}

void init_gui_rendering()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, DEF_GUI_W, DEF_GUI_H, 0, -20, 20);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void init_rendering_to_texture()
{
    glGenTextures(1, &tex_fb);
    glBindTexture(GL_TEXTURE_2D, tex_fb);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 350, 47, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void glVertexf2(float2 vex)
{
	glVertex2f(vex.x, vex.y);
}

void render_texture_rot(Gl_texture* tex, float x, float y, float ang)
{
	double2 p1 = {.x = -tex->img->width*0.5, .y =  tex->img->height*0.5};
	double2 p2 = {.x =  tex->img->width*0.5, .y =  tex->img->height*0.5};
	double2 p3 = {.x =  tex->img->width*0.5, .y = -tex->img->height*0.5};
	double2 p4 = {.x = -tex->img->width*0.5, .y = -tex->img->height*0.5};

	p1 = rotate_point(p1, ang);
	p2 = rotate_point(p2, ang);
	p3 = rotate_point(p3, ang);
	p4 = rotate_point(p4, ang);

    glEnable(GL_TEXTURE_2D);
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1); glVertex2f(x + p1.x, y + p1.y);
    glTexCoord2f(1, 1); glVertex2f(x + p2.x, y + p2.y);
    glTexCoord2f(1, 0); glVertex2f(x + p3.x, y + p3.y);
    glTexCoord2f(0, 0); glVertex2f(x + p4.x, y + p4.y);

    glEnd();
}

void render_texture_rot_point(Gl_texture* tex, float x, float y, float ang, float center_x, float center_y)
{
	double2 p1 = {.x = -tex->img->width*0.5-center_x, .y =  tex->img->height*0.5-center_y};
	double2 p2 = {.x =  tex->img->width*0.5-center_x, .y =  tex->img->height*0.5-center_y};
	double2 p3 = {.x =  tex->img->width*0.5-center_x, .y = -tex->img->height*0.5-center_y};
	double2 p4 = {.x = -tex->img->width*0.5-center_x, .y = -tex->img->height*0.5-center_y};

	p1 = rotate_point(p1, ang);
	p2 = rotate_point(p2, ang);
	p3 = rotate_point(p3, ang);
	p4 = rotate_point(p4, ang);

    glEnable(GL_TEXTURE_2D);
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1); glVertex2f(x + p1.x+center_x, y + p1.y+center_y);
    glTexCoord2f(1, 1); glVertex2f(x + p2.x+center_x, y + p2.y+center_y);
    glTexCoord2f(1, 0); glVertex2f(x + p3.x+center_x, y + p3.y+center_y);
    glTexCoord2f(0, 0); glVertex2f(x + p4.x+center_x, y + p4.y+center_y);

    glEnd();
}

void render_texture_rot_point_scaled(Gl_texture* tex, float x, float y, float ang, float center_x, float center_y, float scale)
{
	double2 p1 = {.x = (x-tex->img->width*scale*0.5-center_x), .y = (y+tex->img->height*scale*0.5-center_y)};
	double2 p2 = {.x = (x+tex->img->width*scale*0.5-center_x), .y = (y+tex->img->height*scale*0.5-center_y)};
	double2 p3 = {.x = (x+tex->img->width*scale*0.5-center_x), .y = (y-tex->img->height*scale*0.5-center_y)};
	double2 p4 = {.x = (x-tex->img->width*scale*0.5-center_x), .y = (y-tex->img->height*scale*0.5-center_y)};

	p1 = rotate_point(p1, ang);
	p2 = rotate_point(p2, ang);
	p3 = rotate_point(p3, ang);
	p4 = rotate_point(p4, ang);

    glEnable(GL_TEXTURE_2D);
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1); glVertex2f(p1.x+center_x, p1.y+center_y);
    glTexCoord2f(1, 1); glVertex2f(p2.x+center_x, p2.y+center_y);
    glTexCoord2f(1, 0); glVertex2f(p3.x+center_x, p3.y+center_y);
    glTexCoord2f(0, 0); glVertex2f(p4.x+center_x, p4.y+center_y);

    glEnd();
}

void render_texture_frag_rot(Gl_texture* tex, float x1, float y1, float x2, float y2, float x, float y, float ang)
{
	float scale_x = x2 - x1;
	float scale_y = y2 - y1;

	double2 p1 = {.x = -tex->img->width*0.5, .y =  tex->img->height*0.5};
	double2 p2 = {.x =  tex->img->width*0.5, .y =  tex->img->height*0.5};
	double2 p3 = {.x =  tex->img->width*0.5, .y = -tex->img->height*0.5};
	double2 p4 = {.x = -tex->img->width*0.5, .y = -tex->img->height*0.5};

	p1 = rotate_point(p1, ang);
	p2 = rotate_point(p2, ang);
	p3 = rotate_point(p3, ang);
	p4 = rotate_point(p4, ang);

	p1.x *= scale_x;
	p2.x *= scale_x;
	p3.x *= scale_x;
	p4.x *= scale_x;
	p1.y *= scale_y;
	p2.y *= scale_y;
	p3.y *= scale_y;
	p4.y *= scale_y;

    glEnable(GL_TEXTURE_2D);
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

    glTexCoord2f(x1, y2); glVertex2f(x + p1.x, y + p1.y);
    glTexCoord2f(x2, y2); glVertex2f(x + p2.x, y + p2.y);
    glTexCoord2f(x2, y1); glVertex2f(x + p3.x, y + p3.y);
    glTexCoord2f(x1, y1); glVertex2f(x + p4.x, y + p4.y);

    glEnd();
}

void glVertex2f_rc(float x, float y, float angle)
{
    x -= 275;
    y -= 200;

    float new_x = cos(angle)*x - sin(angle)*y + DEF_WIN_W*0.5;
    float new_y = sin(angle)*x + cos(angle)*y + DEF_WIN_H*0.5;

    glVertex2f(new_x, new_y);
}

void render_texture_rot_center(Gl_texture* tex, float x1, float y1, float x2, float y2, float angle, int rot)
{
    glEnable(GL_TEXTURE_2D);
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

	if(rot == 0)
	{
		glTexCoord2f(0, 1); glVertex2f_rc(x1, y2, angle);
		glTexCoord2f(1, 1); glVertex2f_rc(x2, y2, angle);
		glTexCoord2f(1, 0); glVertex2f_rc(x2, y1, angle);
		glTexCoord2f(0, 0); glVertex2f_rc(x1, y1, angle);
	}
	else if(rot == 1)
	{
		glTexCoord2f(1, 1); glVertex2f_rc(x1, y2, angle);
		glTexCoord2f(1, 0); glVertex2f_rc(x2, y2, angle);
		glTexCoord2f(0, 0); glVertex2f_rc(x2, y1, angle);
		glTexCoord2f(0, 1); glVertex2f_rc(x1, y1, angle);
	}
	else if(rot == 2)
	{
		glTexCoord2f(1, 0); glVertex2f_rc(x1, y2, angle);
		glTexCoord2f(0, 0); glVertex2f_rc(x2, y2, angle);
		glTexCoord2f(0, 1); glVertex2f_rc(x2, y1, angle);
		glTexCoord2f(1, 1); glVertex2f_rc(x1, y1, angle);
	}
	else if(rot == 3)
	{
		glTexCoord2f(0, 0); glVertex2f_rc(x1, y2, angle);
		glTexCoord2f(0, 1); glVertex2f_rc(x2, y2, angle);
		glTexCoord2f(1, 1); glVertex2f_rc(x2, y1, angle);
		glTexCoord2f(1, 0); glVertex2f_rc(x1, y1, angle);
	}

    glEnd();
}

void render_texture(Gl_texture* tex, float x, float y, float scale, bool reversed)
{
    gtex_bind_texture(tex);

    float half_w = tex->img->width/2.0;
    float half_h = tex->img->height/2.0;

    if(reversed)
    {
        glBegin(GL_QUADS);

        glTexCoord2f(1, 1); glVertex2f(x-half_w*scale, y+half_h*scale);
        glTexCoord2f(0, 1); glVertex2f(x+half_w*scale, y+half_h*scale);
        glTexCoord2f(0, 0); glVertex2f(x+half_w*scale, y-half_h*scale);
        glTexCoord2f(1, 0); glVertex2f(x-half_w*scale, y-half_h*scale);

        glEnd();
    }
    else
    {
        glBegin(GL_QUADS);

        glTexCoord2f(0, 1); glVertex2f(x-half_w*scale, y+half_h*scale);
        glTexCoord2f(1, 1); glVertex2f(x+half_w*scale, y+half_h*scale);
        glTexCoord2f(1, 0); glVertex2f(x+half_w*scale, y-half_h*scale);
        glTexCoord2f(0, 0); glVertex2f(x-half_w*scale, y-half_h*scale);

        glEnd();
    }
}

void render_texture_fr(Gl_texture* tex, float x, float y, float scale, bool reversed, float rot_center_x, float rot_center_y, float angle)
{
    gtex_bind_texture(tex);

    float half_w = tex->img->width/2.0;
    float half_h = tex->img->height/2.0;

    if(reversed)
    {
        glBegin(GL_QUADS);

        glTexCoord2f(1, 1); glVertexf2(rotate_vertex((float2){x-half_w*scale, y+half_h*scale}, (float2){rot_center_x, rot_center_y}, angle));
        glTexCoord2f(0, 1); glVertexf2(rotate_vertex((float2){x+half_w*scale, y+half_h*scale}, (float2){rot_center_x, rot_center_y}, angle));
        glTexCoord2f(0, 0); glVertexf2(rotate_vertex((float2){x+half_w*scale, y-half_h*scale}, (float2){rot_center_x, rot_center_y}, angle));
        glTexCoord2f(1, 0); glVertexf2(rotate_vertex((float2){x-half_w*scale, y-half_h*scale}, (float2){rot_center_x, rot_center_y}, angle));

        glEnd();
    }
    else
    {
        glBegin(GL_QUADS);

        glTexCoord2f(0, 1); glVertexf2(rotate_vertex((float2){x-half_w*scale, y+half_h*scale}, (float2){rot_center_x, rot_center_y}, angle));
        glTexCoord2f(1, 1); glVertexf2(rotate_vertex((float2){x+half_w*scale, y+half_h*scale}, (float2){rot_center_x, rot_center_y}, angle));
        glTexCoord2f(1, 0); glVertexf2(rotate_vertex((float2){x+half_w*scale, y-half_h*scale}, (float2){rot_center_x, rot_center_y}, angle));
        glTexCoord2f(0, 0); glVertexf2(rotate_vertex((float2){x-half_w*scale, y-half_h*scale}, (float2){rot_center_x, rot_center_y}, angle));

        glEnd();
    }
}

void render_color_rect(float x1, float y1, float x2, float y2, float r, float g, float b, float a)
{
    glDisable(GL_TEXTURE_2D);
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);

    glVertex2f(x1, y2);
    glVertex2f(x2, y2);
    glVertex2f(x2, y1);
    glVertex2f(x1, y1);

    glEnd();
    glEnable(GL_TEXTURE_2D);
    glColor4f(1, 1, 1, 1);
}

void render_color_rect_with_frame(float x1, float y1, float w, float h, int color, int frame_color, float thickness)
{
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    float x2 = x1+w;
    float y2 = y1+h;

    glColor3ub(frame_color >> 16, (frame_color >> 8) & 255, frame_color & 255);
    glVertex2f(x1, y2);
    glVertex2f(x2, y2);
    glVertex2f(x2, y1);
    glVertex2f(x1, y1);

    glColor3ub(color >> 16, (color >> 8) & 255, color & 255);
    glVertex2f(x1+thickness, y2-thickness);
    glVertex2f(x2-thickness, y2-thickness);
    glVertex2f(x2-thickness, y1+thickness);
    glVertex2f(x1+thickness, y1+thickness);

    glEnd();
    glEnable(GL_TEXTURE_2D);
    glColor4f(1, 1, 1, 1);
}

void render_color_rect_wh(float x1, float y1, float w, float h, int color)
{
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    float x2 = x1+w;
    float y2 = y1+h;

    glColor3ub(color >> 16, (color >> 8) & 255, color & 255);
    glVertex2f(x1, y2);
    glVertex2f(x2, y2);
    glVertex2f(x2, y1);
    glVertex2f(x1, y1);

    glEnd();
    glEnable(GL_TEXTURE_2D);
    glColor4f(1, 1, 1, 1);
}

void render_color_rect_whf(float x1, float y1, float w, float h, float r, float g, float b)
{
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    float x2 = x1+w;
    float y2 = y1+h;

    glColor3f(r, g, b);
    glVertex2f(x1, y2);
    glVertex2f(x2, y2);
    glVertex2f(x2, y1);
    glVertex2f(x1, y1);

    glEnd();
    glEnable(GL_TEXTURE_2D);
    glColor4f(1, 1, 1, 1);
}
