#include <allegro5/allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "new_img_lib.h"

#define debugPrint printf

typedef struct Col_rgb bit_amount_t;

static int array3d(int x, int y, int z, int w, int d)
{
    return y*w*d + x*d + z;
}

Image_bmp* iml_create_image_struct(int width, int height, int bpp)
{
    Image_bmp* ret = malloc(sizeof(Image_bmp));
    ret->width = width;
    ret->height = height;
    ret->bits_per_px = bpp;
    ret->pixel = calloc(width*height*bpp/8, 1);
    return ret;
}

void iml_destroy_image_struct(Image_bmp* ptr)
{
    free(ptr->pixel);
    free(ptr);
}

void iml_clear_image_to_color(Image_bmp* ptr, byte r, byte g, byte b, byte a)
{
    if(ptr->bits_per_px == 24)
    {
        for(int y=0; y<ptr->height; y++)
        {
            for(int x=0; x<ptr->width; x++)
            {
                iml_set_image_pixel_rgb(ptr, x, y, r, g, b);
            }
        }
    }
    else if(ptr->bits_per_px == 32)
    {
        for(int y=0; y<ptr->height; y++)
        {
            for(int x=0; x<ptr->width; x++)
            {
                iml_set_image_pixel_rgba(ptr, x, y, r, g, b, a);
            }
        }
    }
}

Image_bmp* iml_load_image_from_png(const char* filename, int flags)
{
    ALLEGRO_BITMAP* bitmap = al_load_bitmap(filename);
    if(bitmap == NULL)
    {
        return NULL;
    }

	ALLEGRO_LOCKED_REGION* region = al_lock_bitmap(bitmap, al_get_bitmap_format(bitmap), ALLEGRO_LOCK_READONLY);
	int bitmap_w = al_get_bitmap_width(bitmap);
	int bitmap_h = al_get_bitmap_height(bitmap);
	Image_bmp* ret = iml_create_image_struct(bitmap_w, bitmap_h, region->pixel_size*8);
	byte* region_data = region->data;

	int px_counter = -(bitmap_h-1)*bitmap_w*region->pixel_size;
	for(int y=bitmap_h-1; y>=0; y--)
    {
        for(int x=0; x<bitmap_w; x++)
        {
            byte b = region_data[px_counter];
            byte g = region_data[px_counter+1];
            byte r = region_data[px_counter+2];
            byte a = region_data[px_counter+3];

            if(x < 0 || x >= bitmap_w) printf("ERROR x %i\n", x);
            if(y < 0 || y >= bitmap_h) printf("ERROR y %i\n", y);
            iml_set_image_pixel_rgba(ret, x, y, r, g, b, a);
            px_counter += 4;
        }
    }

    al_destroy_bitmap(bitmap);
	return ret;
}

rgb_t get_image_avg_color(Image_bmp* img)
{
	int r_sum = 0;
	int g_sum = 0;
	int b_sum = 0;
	int px_count = img->width*img->height;

	for(int y=0; y<img->height; y++)
	{
		for(int x=0; x<img->width; x++)
		{
			rgba_t color = iml_get_image_pixel_rgba(img, x, y);
			r_sum += color.r;
			g_sum += color.g;
			b_sum += color.b;
		}
	}

	return (rgb_t) {r_sum/px_count, g_sum/px_count, b_sum/px_count};
}

void iml_swap_color_rb(rgb_t* col)
{
    byte buffer = col->r;
    col->r = col->b;
    col->b = buffer;
}

void iml_swap_color_rba(rgba_t* col)
{
    byte buffer = col->r;
    col->r = col->b;
    col->b = buffer;
}

rgb_t iml_get_image_pixel_rgb(const Image_bmp* ptr, int x, int y)
{
    if(x < 0 || x >= ptr->width || y < 0 || y >= ptr->height)
    {
        return (rgb_t) {0, 0, 0};
    }

    int px = array3d(x, y, 0, ptr->width, 3);
    return (rgb_t)
    {
        ptr->pixel[px],
        ptr->pixel[px+1],
        ptr->pixel[px+2]
    };
}

rgba_t iml_get_image_pixel_rgba(const Image_bmp* ptr, int x, int y)
{
    int px = array3d(x, y, 0, ptr->width, 4);
    return (rgba_t)
    {
        ptr->pixel[px],
        ptr->pixel[px+1],
        ptr->pixel[px+2],
        ptr->pixel[px+3]
    };
}

void iml_set_image_pixel_rgb(Image_bmp* ptr, int x, int y, byte r, byte g, byte b)
{
    int px = array3d(x, y, 0, ptr->width, 3);
    ptr->pixel[px] = r;
    ptr->pixel[px+1] = g;
    ptr->pixel[px+2] = b;
}

void iml_set_image_pixel_rgba(Image_bmp* ptr, int x, int y, byte r, byte g, byte b, byte a)
{
    int px = array3d(x, y, 0, ptr->width, 4);
    ptr->pixel[px] = r;
    ptr->pixel[px+1] = g;
    ptr->pixel[px+2] = b;
    ptr->pixel[px+3] = a;
}

void iml_set_image_pixel_a(Image_bmp* ptr, int x, int y, byte a)
{
    int px = array3d(x, y, 0, ptr->width, 4);
    ptr->pixel[px+3] = a;
}

void iml_change_image_pixel_a(Image_bmp* ptr, int x, int y, short a)
{
    int px = array3d(x, y, 0, ptr->width, 4);
    ptr->pixel[px+3] += a;
}

void iml_fill_image_randomly(Image_bmp* ptr)
{
    if(ptr->bits_per_px == 24)
    {
        for(int y=0; y<ptr->height; y++)
        {
            for(int x=0; x<ptr->width; x++)
            {
                iml_set_image_pixel_rgb(ptr, x, y, rand()%256, rand()%256, rand()%256);
            }
        }
    }
    else if(ptr->bits_per_px == 32)
    {
        for(int y=0; y<ptr->height; y++)
        {
            for(int x=0; x<ptr->width; x++)
            {
                iml_set_image_pixel_rgba(ptr, x, y, rand()%256, rand()%256, rand()%256, 255);
            }
        }
    }
}

void convert_color_to_hex(rgba_t color, char* ret)
{
    char num_to_hex(byte n)
    {
        if(n < 10) return n+'0';
        return n-10+'a';
    }

    ret[0] = num_to_hex(color.r/16);
    ret[1] = num_to_hex(color.r%16);
    ret[2] = num_to_hex(color.g/16);
    ret[3] = num_to_hex(color.g%16);
    ret[4] = num_to_hex(color.b/16);
    ret[5] = num_to_hex(color.b%16);
}

