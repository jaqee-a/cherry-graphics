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

void procedural_island()
{
    cherry_fill(pixels, WIDTH, HEIGHT, 0xFFFFFFFF);

    float max_d=(WIDTH/2)*(WIDTH/2)+(HEIGHT/2)*(HEIGHT/2);
    float radius=0.25;
    
    for(size_t y=0; y<HEIGHT; y+=1)
    for(size_t x=0; x<WIDTH; x+=1)
    {
        float dx=(float)x-WIDTH/2;
        float dy=(float)y-HEIGHT/2;
        float dist_c=((dx*dx)+(dy*dy))/max_d;
        float color=1-smoothstep(radius-0.3, radius+0.3, dist_c);
        float n = noise(x/100.0, y/100.0, 0)*color;
        int32_t v;

        if(n<0.03)
        {
            v=0xFF003C5F;
        }else if(n<0.14){
            uint32_t color  = 0xFF003C5F;
            uint32_t t_color= 0xFF005EB8;
            v=lerpc(color, t_color, (n-0.03)/(0.14-0.03));
        }else if(n<0.17){
            v=0xFF005EB8;
        }else if(n<0.19){
            uint32_t color  = 0xFF005EB8;
            uint32_t t_color= 0xFFC2B280;
            v=lerpc(color, t_color, (n-0.17)/(0.19-0.17));
        }else if(n<0.29){
            v=0xFFC2B280;
        }else if(n<0.32){
            uint32_t color  = 0xFFC2B280;
            uint32_t t_color= 0xFF4FAD27;
            v=lerpc(color, t_color, (n-0.29)/(0.32-0.29));
        }else if(n<0.45){
            v=0xFF4FAD27;
        }else if(n<1.0){
            v=0xFF778899;
        }
        pixels[y*WIDTH+x]=v;
    }

/*
    // ODD
    int32_t kernel=9;
    int32_t half_k=(kernel-1)/2;
    int32_t p_kernel=kernel*kernel;
    uint32_t sum_r, sum_g, sum_b;
    // Apply gaussian blur
    for(size_t y=half_k; y<HEIGHT-half_k; y+=1)
    for(size_t x=half_k; x<WIDTH-half_k; x+=1)
    {
        sum_r=sum_g=sum_b=0;
        for(int32_t i=-half_k; i<half_k+1; i+=1)
        for(int32_t j=-half_k; j<half_k+1; j+=1)
        {
            uint32_t color=pixels[(y+i)*WIDTH+(x+j)];
            // printf("%zu\n", color);

            sum_r = sum_r+      (color&0xFF);
            sum_g = sum_g+ ((color>>8)&0xFF);
            sum_b = sum_b+((color>>16)&0xFF);
        }
        sum_r/=p_kernel;
        sum_g/=p_kernel;
        sum_b/=p_kernel;
        pixels[y*WIDTH+x]=sum_r+(sum_g<<8)+(sum_b<<16)+0xFF000000;
    }
*/
    const char* file_path = "examples/procedural_island_example.bmp";
    Errno err = cherry_save_to_bmp(pixels, COLOR_MODE_BGRA, WIDTH, HEIGHT, file_path);
    if(err) {
        fprintf(stderr, "Could write to file %s: %s", file_path, strerror(err));
    }
}

int main(void)
{
    // example_test_all();
    // example_perlin();
    procedural_island();
    
    return 0;
}