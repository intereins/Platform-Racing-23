#ifndef GL_TEXTURE_H_INCLUDED
#define GL_TEXTURE_H_INCLUDED

#include <allegro5/allegro.h>
#include <allegro5/allegro_opengl.h>
#include "new_img_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Gl_texture
{
    Image_bmp* img;
    unsigned int id;
};

struct gl_texture
{
    int width, height;
    GLubyte *texture;
    unsigned int id;
};

typedef struct gl_texture gl_texture;
typedef struct Gl_texture Gl_texture;

Gl_texture* gtex_create_texture(Image_bmp* img);
Gl_texture* gtex_create_texture_and_img(int width, int height, int bpp);
Gl_texture* gtex_load_texture(const char* filename);
void gtex_update_texture(Gl_texture* tex);
void gtex_destroy_texture(Gl_texture* texture);
void gtex_bind_texture(Gl_texture* tex);
void gtex_render_texture(Gl_texture* tex, float x1, float y1, float x2, float y2);
void gtex_render_texture_wh(Gl_texture *tex, float x, float y, float w, float h);
void gtex_render_texture_def_size(Gl_texture *tex, float x, float y);
void gtex_render_texture_frag_def_size(Gl_texture *tex, float x, float y, float x1, float y1, float x2, float y2);
void gtex_render_texture_frag_def_size_invh(Gl_texture *tex, float x, float y, float x1, float y1, float x2, float y2);
void gtex_render_texture_inv(Gl_texture *tex, float x1, float y1, float x2, float y2);
void gtex_render_texture_rot90(Gl_texture *tex, float x1, float y1, float x2, float y2);
void gtex_render_texture_rot270(Gl_texture* tex, float x1, float y1, float x2, float y2);
void gtex_render_texture_frag(Gl_texture* tex, float x1, float y1, float x2, float y2, float tx1, float ty1, float tx2, float ty2);

void bind_texture(gl_texture* tx_ptr);
void load_texture(gl_texture* tx_ptr, const char* filename);

#ifdef __cplusplus
}
#endif

#endif // GL_TEXTURE_H_INCLUDED
