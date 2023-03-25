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
}

int main(void)
{
    example_test_all();

    const char* file_path = "examples/output_example.bmp";
    Errno err = cherry_save_to_bmp(pixels, COLOR_MODE_RGBA, WIDTH, HEIGHT, file_path);
    if(err) {
        fprintf(stderr, "Could write to file %s: %s", file_path, strerror(err));
    }
    return 0;
}