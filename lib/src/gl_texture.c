#include <stdio.h>
#include "gl_texture.h"

static int array3d(int x, int y, int z, int depth, int width)
{
    return width*depth*y + depth*x + z;
}

Gl_texture* gtex_create_texture(Image_bmp* img)
{
    Gl_texture* ret = malloc(sizeof(Gl_texture));
    ret->img = img;

    glGenTextures(1, &ret->id);
    glBindTexture(GL_TEXTURE_2D, ret->id);

    if(img->bits_per_px == 24)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width, img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixel);
    }
    else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixel);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return ret;
}

Gl_texture* gtex_create_texture_and_img(int width, int height, int bpp)
{
    Gl_texture* ret = malloc(sizeof(Gl_texture));
    Image_bmp* img = iml_create_image_struct(width, height, bpp);
    ret->img = img;

    glGenTextures(1, &ret->id);
    glBindTexture(GL_TEXTURE_2D, ret->id);

    if(img->bits_per_px == 24)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width, img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixel);
    }
    else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixel);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return ret;
}

Gl_texture* gtex_load_texture(const char* filename)
{
    int filename_len = strlen(filename);
    Gl_texture* ret = malloc(sizeof(Gl_texture));

    ret->img = iml_load_image_from_png(filename, 0);
    if(ret->img == NULL)
    {
        free(ret);
        return NULL;
    }

    glGenTextures(1, &ret->id);
    glBindTexture(GL_TEXTURE_2D, ret->id);

    if(ret->img->bits_per_px == 24)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ret->img->width, ret->img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, ret->img->pixel);
    }
    else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ret->img->width, ret->img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ret->img->pixel);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return ret;
}

Gl_texture* gtex_load_texture_png(const char* filename)
{
    Gl_texture* ret = malloc(sizeof(Gl_texture));
    ret->img = iml_load_image_from_png(filename, 0);

    glGenTextures(1, &ret->id);
    glBindTexture(GL_TEXTURE_2D, ret->id);

    if(ret->img->bits_per_px == 24)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ret->img->width, ret->img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, ret->img->pixel);
    }
    else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ret->img->width, ret->img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ret->img->pixel);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return ret;
}

void gtex_update_texture(Gl_texture* tex)
{
    gtex_bind_texture(tex);
    if(tex->img->bits_per_px == 24)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->img->width, tex->img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex->img->pixel);
    }
    else glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->img->width, tex->img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->img->pixel);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void gtex_destroy_texture(Gl_texture* texture)
{
    if(texture != NULL)
    {
        iml_destroy_image_struct(texture->img);
        free(texture);
    }
}

void gtex_bind_texture(Gl_texture* tex)
{
    glBindTexture(GL_TEXTURE_2D, tex->id);
}

void bind_texture(gl_texture* tx_ptr)
{
    glBindTexture(GL_TEXTURE_2D, tx_ptr->id);
}

void gtex_render_texture(Gl_texture* tex, float x1, float y1, float x2, float y2)
{
    if(tex == NULL) return;

    glEnable(GL_TEXTURE_2D);
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1); glVertex2f(x1, y2);
    glTexCoord2f(1, 1); glVertex2f(x2, y2);
    glTexCoord2f(1, 0); glVertex2f(x2, y1);
    glTexCoord2f(0, 0); glVertex2f(x1, y1);

    glEnd();
}

void gtex_render_texture_wh(Gl_texture* tex, float x, float y, float w, float h)
{
    if(tex == NULL) return;

    glEnable(GL_TEXTURE_2D);
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1); glVertex2f(x, y+h);
    glTexCoord2f(1, 1); glVertex2f(x+w, y+h);
    glTexCoord2f(1, 0); glVertex2f(x+w, y);
    glTexCoord2f(0, 0); glVertex2f(x, y);

    glEnd();
}

void gtex_render_texture_def_size(Gl_texture* tex, float x, float y)
{
    if(tex == NULL) return;

    glEnable(GL_TEXTURE_2D);
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1); glVertex2f(x, y+tex->img->height);
    glTexCoord2f(1, 1); glVertex2f(x+tex->img->width, y+tex->img->height);
    glTexCoord2f(1, 0); glVertex2f(x+tex->img->width, y);
    glTexCoord2f(0, 0); glVertex2f(x, y);

    glEnd();
}

void gtex_render_texture_frag_def_size(Gl_texture* tex, float x, float y, float x1, float y1, float x2, float y2)
{
    if(tex == NULL) return;

    glEnable(GL_TEXTURE_2D);
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

    glTexCoord2f(x1/tex->img->width, y2/tex->img->height); glVertex2f(x, y+y2-y1);
    glTexCoord2f(x2/tex->img->width, y2/tex->img->height); glVertex2f(x+x2-x1, y+y2-y1);
    glTexCoord2f(x2/tex->img->width, y1/tex->img->height); glVertex2f(x+x2-x1, y);
    glTexCoord2f(x1/tex->img->width, y1/tex->img->height); glVertex2f(x, y);

    glEnd();
}

void gtex_render_texture_frag_def_size_invh(Gl_texture* tex, float x, float y, float x1, float y1, float x2, float y2)
{
    if(tex == NULL) return;

    glEnable(GL_TEXTURE_2D);
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

    glTexCoord2f(x2/tex->img->width, y2/tex->img->height); glVertex2f(x, y+y2-y1);
    glTexCoord2f(x1/tex->img->width, y2/tex->img->height); glVertex2f(x+x2-x1, y+y2-y1);
    glTexCoord2f(x1/tex->img->width, y1/tex->img->height); glVertex2f(x+x2-x1, y);
    glTexCoord2f(x2/tex->img->width, y1/tex->img->height); glVertex2f(x, y);

    glEnd();
}

void gtex_render_texture_inv(Gl_texture* tex, float x1, float y1, float x2, float y2)
{
    if(tex == NULL) return;

    glEnable(GL_TEXTURE_2D);
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

    glTexCoord2f(1, 1); glVertex2f(x1, y2);
    glTexCoord2f(0, 1); glVertex2f(x2, y2);
    glTexCoord2f(0, 0); glVertex2f(x2, y1);
    glTexCoord2f(1, 0); glVertex2f(x1, y1);

    glEnd();
}

void gtex_render_texture_frag(Gl_texture* tex, float x1, float y1, float x2, float y2, float tx1, float ty1, float tx2, float ty2)
{
    glEnable(GL_TEXTURE_2D);
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

    glTexCoord2f(tx1, ty2); glVertex2f(x1, y2);
    glTexCoord2f(tx2, ty2); glVertex2f(x2, y2);
    glTexCoord2f(tx2, ty1); glVertex2f(x2, y1);
    glTexCoord2f(tx1, ty1); glVertex2f(x1, y1);

    glEnd();
}

void gtex_render_texture_rot90(Gl_texture* tex, float x1, float y1, float x2, float y2)
{
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

    glTexCoord2f(1, 1); glVertex2f(x1, y2);
    glTexCoord2f(1, 0); glVertex2f(x2, y2);
    glTexCoord2f(0, 0); glVertex2f(x2, y1);
    glTexCoord2f(0, 1); glVertex2f(x1, y1);

    glEnd();
}

void gtex_render_texture_rot180(Gl_texture* tex, float x1, float y1, float x2, float y2)
{
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 1); glVertex2f(x1, y2);
    glTexCoord2f(1, 1); glVertex2f(x2, y2);
    glTexCoord2f(1, 0); glVertex2f(x2, y1);
    glTexCoord2f(0, 0); glVertex2f(x1, y1);

    glEnd();
}

void gtex_render_texture_rot270(Gl_texture* tex, float x1, float y1, float x2, float y2)
{
    gtex_bind_texture(tex);
    glBegin(GL_QUADS);

    glTexCoord2f(0, 0); glVertex2f(x1, y2);
    glTexCoord2f(0, 1); glVertex2f(x2, y2);
    glTexCoord2f(1, 1); glVertex2f(x2, y1);
    glTexCoord2f(1, 0); glVertex2f(x1, y1);

    glEnd();
}

void load_texture(gl_texture* tx_ptr, const char* filename)
{
    FILE* file = fopen(filename, "rb");
    int px_off, w, h;

    fseek(file, 10, SEEK_SET);
    fread(&px_off, 4, 1, file);

    fseek(file, 18, SEEK_SET);
    fread(&w, 4, 1, file);

    fseek(file, 22, SEEK_SET);
    fread(&h, 4, 1, file);

    tx_ptr->texture = (GLubyte*) malloc(w*h*4*sizeof(GLubyte));

    fseek(file, px_off, SEEK_SET);
    for(int y=h-1; y >= 0; y--)
    {
        for(int x=0; x < w; x++)
        {
            fread(&tx_ptr->texture[array3d(x, y, 2, 4, w)], 1, 1, file);
            fread(&tx_ptr->texture[array3d(x, y, 1, 4, w)], 1, 1, file);
            fread(&tx_ptr->texture[array3d(x, y, 0, 4, w)], 1, 1, file);
            fread(&tx_ptr->texture[array3d(x, y, 3, 4, w)], 1, 1, file);
        }
    }

    fclose(file);

    glGenTextures(1, &tx_ptr->id);
    glBindTexture(GL_TEXTURE_2D, tx_ptr->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tx_ptr->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
