#include "lib/cherry.h"

#include <stdio.h>

#define WIDTH  850
#define HEIGHT 850

static uint32_t pixels[WIDTH*HEIGHT];

void example_test_all()
{
    cherry_fill(pixels, WIDTH, HEIGHT, 0xFFFFFFFF);
    cherry_fill_rect(pixels, WIDTH, HEIGHT, 250, HEIGHT / 2 - 15, 50, 30, 0xFF20FF20);
    cherry_fill_circle(pixels, WIDTH, HEIGHT, 30, 100, 30, 0xFFFF2020);
    cherry_stroke_ellipse(pixels, WIDTH, HEIGHT, 400, 40, 100, 50, 0);
    cherry_stroke_circle(pixels, WIDTH, HEIGHT, 110, 400, 50, 0xFF235154);
    cherry_stroke_line(pixels, WIDTH, HEIGHT, 500, 50, 500, 500, 0xFF324312);

    const char* file_path = "examples/output_example.bmp";
    Errno err = cherry_save_to_bmp(pixels, COLOR_MODE_RGBA, WIDTH, HEIGHT, file_path);
    if(err) {
        fprintf(stderr, "Could write to file %s: %s", file_path, strerror(err));
    }
}

void example_perlin()
{
    cherry_fill(pixels, WIDTH, HEIGHT, 0xFFFFFFFF);

    for(size_t y=0; y<HEIGHT; y+=1)
    for(size_t x=0; x<WIDTH; x+=1)
    {
        int32_t n = noise(x/200.0, y/200.0, 0)*255;
        int32_t v = n+(n<<8)+(n<<16);
        pixels[y*WIDTH+x]=0xFF000000+v;
    }
    
    const char* file_path = "examples/perlin_example.bmp";
    Errno err = cherry_save_to_bmp(pixels, COLOR_MODE_RGBA, WIDTH, HEIGHT, file_path);
    if(err) {
        fprintf(stderr, "Could write to file %s: %s", file_path, strerror(err));
    }
}

int main(void)
{
    example_test_all();
    example_perlin();
    
    return 0;
}