#ifndef NEW_IMG_LIB_H_INCLUDED
#define NEW_IMG_LIB_H_INCLUDED

#include <stdio.h>
#include <stdbool.h>

#define IMLF_DITCH_ALPHA 1

#ifndef BYTE_DEFINED
typedef unsigned char byte;
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct Col_rgb
{
    byte r, g, b;
};

struct Col_rgba
{
    byte r, g, b, a;
};

struct Colf_rgb
{
    float r, g, b;
};

struct Colf_rgba
{
    float r, g, b, a;
};

struct Bit_buffer
{
    bool* data;
    int length;
    int counter;
};

struct Image_bmp
{
    byte* pixel;
    int width;
    int height;
    int bits_per_px;
};

typedef struct Col_rgb rgb_t;
typedef struct Col_rgba rgba_t;
typedef struct Colf_rgb rgbf_t;
typedef struct Colf_rgb rgbaf_t;
typedef struct Image_bmp Image_bmp;
typedef struct Image_bmp Pixel_buffer;
typedef struct Bit_buffer Bit_buffer;

void iml_save_image_to_bmp(Image_bmp* ptr, const char* filename);
void iml_clear_image_to_color(Image_bmp* ptr, byte r, byte g, byte b, byte a);
void iml_swap_color_rb(rgb_t* col);
void iml_swap_color_rba(rgba_t* col);
void iml_print_bmp_header_to_file(FILE* file, int width, int height, short bpp);
void iml_set_image_pixel_rgba(Image_bmp* ptr, int x, int y, byte r, byte g, byte b, byte a);
void iml_set_image_pixel_rgb(Image_bmp* ptr, int x, int y, byte r, byte g, byte b);
void iml_set_image_pixel_a(Image_bmp* ptr, int x, int y, byte a);
void iml_change_image_pixel_a(Image_bmp* ptr, int x, int y, short a);
void iml_destroy_image_struct(Image_bmp* ptr);
void iml_fill_image_randomly(Image_bmp* ptr);
void iml_copy_img_into_another(Image_bmp* dest, const Image_bmp* src, int dest_x, int dest_y);
void iml_remove_color(Image_bmp* img, rgb_t col_rem, rgb_t col_bg);

rgb_t iml_get_image_pixel_rgb(const Image_bmp* ptr, int x, int y);
rgba_t iml_get_image_pixel_rgba(const Image_bmp* ptr, int x, int y);
rgb_t get_image_avg_color(Image_bmp *img);

Image_bmp* iml_create_image_struct(int width, int height, int bpp);
Image_bmp* iml_load_image_from_bmp(const char* filename, int flags);
Image_bmp* iml_load_image_from_png(const char* filename, int flags);
Image_bmp *iml_determine_alpha(Image_bmp *img_b, Image_bmp *img_w);
Image_bmp* iml_extract_image(Image_bmp* img, int x, int y, int w, int h);
Image_bmp* iml_scale_img(const Image_bmp* img, int scale);
Image_bmp* iml_downscale_img(const Image_bmp* img, int scale);

//satctic
rgb_t find_lowest_color_on_frag_rgb(Image_bmp* img, int x, int y, int w, int h);
int compress_frag_lowest(Image_bmp* img, int pos_x, int pos_y, int w, int h, Bit_buffer* buffer);
int compress_frag_color_list(Image_bmp* img, int pos_x, int pos_y, int w, int h, Bit_buffer* buffer);
void convert_to_color_list(Image_bmp* img, const char* filename);

#ifdef __cplusplus
}
#endif

#endif // NEW_IMG_LIB_H_INCLUDED
