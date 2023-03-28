#ifndef CHERRY_C_
#define CHERRY_C_

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>

#define PI 3.14159265358979323846

#define RAD2DEG (180.0/PI)
#define DEG2RAD (PI/180.0)

#include "noise.h"

#define COLOR_MODE_RGB  1
#define COLOR_MODE_BGR  2
#define COLOR_MODE_RGBA 3
#define COLOR_MODE_BGRA 4


typedef int Errno;

// Utils
float lerpf(float a, float b, float t) { return a+(b-a)*t; }
float clamp(float edge_1, float edge_2, float x) { return x<edge_1?edge_1:(x>edge_2?edge_2:x); }
float step(float edge, float x) { return x>=edge?1:0; }
float smoothstep(float edge_1, float edge_2, float x) { return (clamp(edge_1, edge_2, x)-edge_1)/(edge_2-edge_1); }
uint32_t lerpc(uint32_t a, uint32_t b, float t)
{
    uint8_t red_c   = ( a&0xFF);
    uint8_t green_c = ((a>>8)&0xFF);
    uint8_t blue_c  = ((a>>16)&0xFF);

    uint8_t red_t   = ( b&0xFF);
    uint8_t green_t = ((b>>8)&0xFF);
    uint8_t blue_t  = ((b>>16)&0xFF);

    uint8_t red   = (uint8_t)lerpf((float)red_c,  (float) red_t,   t);
    uint8_t green = (uint8_t)lerpf((float)green_c,(float) green_t, t);
    uint8_t blue  = (uint8_t)lerpf((float)blue_c, (float) blue_t,  t);

    return red+(green<<8)+(blue<<16)+0xFF000000;
}
// Ref: https://mathopenref.com/coordparamellipse.html
void cherry_stroke_ellipse(uint32_t *pixels, size_t width, size_t height,
                           int32_t cx, int32_t cy, size_t rx, size_t ry, uint32_t color)
{
    // x = cx + a * cos(t)
    // y = cy + b * sin(t)

    for(size_t t=0; t<360; t+=1)
    {
        int32_t x=cx+rx*cos(t);
        if(0>x||(int32_t)width<=x) continue;

        int32_t y=cy+ry*sin(t);
        if(0>y||(int32_t)height<=y) continue;

        pixels[y*(int32_t)width+x]=color;
    }

}

void cherry_stroke_circle(uint32_t *pixels, size_t width, size_t height,
                          int32_t cx, int32_t cy, size_t r, uint32_t color)
{
    for(size_t t=0; t<360; t+=1)
    {
        int x=cx+r*cos(t);
        if(0>x||(int32_t)width<=x) continue;

        int y=cy+r*sin(t);
        if(0>y||(int32_t)height<=y) continue;

        pixels[y*(int32_t)width+x] = color;
    }
}

// Ref: https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
// TODO: Fix point starting from outside the canvas
void cherry_stroke_line(uint32_t *pixels, size_t width, size_t height,
                        int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
{
    int32_t dx=x1-x0;
    int32_t dy=y1-y0;
    
    int32_t D=2*dy-dx;
    int32_t y=y0;

    for(int32_t x=x0; x<x1; x+=1)
    {
        if(0>y||(int32_t)height<=y||0>x||(int32_t)width<=x) continue;

        pixels[y*(int32_t)width+x]=color;
        if (D > 0)
        {
            y=y+1;
            D=D-2*dx;
        }
        D=D+2*dy;
    }
}

void cherry_fill(uint32_t *pixels, size_t width, size_t height, uint32_t color)
{
    for(size_t i=0; i<width*height; i+=1)
    {
        pixels[i]=color;
    }
}

void cherry_fill_ellipse(uint32_t *pixels, size_t width, size_t height,
                           int32_t cx, int32_t cy, size_t rx, size_t ry, uint32_t color)
{
    float F=sqrt(abs(ry*ry-rx*rx));

    int32_t FX1=cx;
    int32_t FX2=cx;

    int32_t FY1=cy;
    int32_t FY2=cy;

    if(rx>ry)
    {
        FX1+=F;
        FX2-=F;
    }else{
        FY1+=F;
        FY2-=F;
    }

    int32_t ex=cx+rx;
    int32_t ey=cy;

    int32_t dex1=ex-FX1;
    int32_t dex2=ex-FX2;

    int32_t dey1=ey-FY1;
    int32_t dey2=ey-FY2;

    float n_dist=sqrt(dex1*dex1+dey1*dey1)+sqrt(dex2*dex2+dey2*dey2);

    for(int32_t y=cy-ry; y<cy+(int32_t)rx; y+=1)
    {
        if(0>y||(int32_t)height<=y) continue;

        for(int32_t x=cx-rx; x<cx+(int32_t)rx; x+=1)
        {
            if(0>x||(int32_t)width<=x) continue;

            int32_t px1=x-FX1;
            int32_t px2=x-FX2;

            int32_t py1=y-FY1;
            int32_t py2=y-FY2;

            if(sqrt(px1*px1+py1*py1)+sqrt(px2*px2+py2*py2)<n_dist)
            {
                pixels[y*(int32_t)width+x]=color;
            }
        }
    }

}

void cherry_fill_rect(uint32_t *pixels, size_t width, size_t height,
                      int32_t x, int32_t y, size_t rw, size_t rh, uint32_t color)
{
    for(int32_t py=y; py<y+(int32_t)rh; py+=1)
    {
        if(0>py||(int32_t)height<=py) continue;

        for(int32_t dx=x; dx<x+(int32_t)rw; dx+=1)
        {
            if(0>dx||(int32_t)width<=dx) continue; 

            pixels[py*(int32_t)width+dx]=color;
        }
    }
}

void swap_int(int32_t *i1, int32_t *i2)
{
    int32_t tmp = (*i1);
    *i1=*i2;
    *i2=tmp;
}

void get_line_segments(int32_t x0, int32_t y0,
                       int32_t x1, int32_t y1,
                       float *m  , int32_t *b)
{
    int32_t dx=x0-x1;
    int32_t dy=y0-y1;

    if(dx == 0){
        *m=0;
        *b=x0;
        return;
    }
        
    *m = (float)dy/dx;
    *b = y0-*m*x0;
}

void sort_points_by_y(int32_t *x0, int32_t *y0, int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2)
{
    if(*y1 > *y0)
    {
        swap_int(y1, y0);
        swap_int(x1, x0);
    }

    if(*y2 > *y1)
    {
        swap_int(y1, y2);
        swap_int(x1, x2);
    }

    if(*y2 > *y0)
    {
        swap_int(y0, y2);
        swap_int(x0, x2);
    }
}

void cherry_fill_triangle(uint32_t *pixels, size_t width, size_t height,
                          int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
{
    /*
        Line function
        y=mx+b
        x=(y-b)/m
        TOP TO MIDDLE LINE
        ytm=m(xtm)+b
        
        y0=m(x0)+b
        y1=m(x1)+b

        y0-y1
        ----- = m
        x0-x1

        b=y0-m(x0)
    */

   sort_points_by_y(&x0, &y0, &x1, &y1, &x2, &y2); 

    float m_top_bottom, m_top_middle, m_middle_bottom;
    int32_t b_top_middle, b_top_bottom, b_middle_bottom;

    get_line_segments(x0, y0, x1, y1, &m_top_middle, &b_top_middle);
    get_line_segments(x0, y0, x2, y2, &m_top_bottom, &b_top_bottom);
    get_line_segments(x1, y1, x2, y2, &m_middle_bottom, &b_middle_bottom);
 
    for(int32_t y=y2; y<y1; y+=1)
    {
        if(0>y||(int32_t)height<=y) continue;

        int32_t x_middle_bottom=m_middle_bottom?(y-b_middle_bottom)/m_middle_bottom:b_middle_bottom;
        int32_t x_top_bottom   =m_top_bottom?(y-b_top_bottom)/m_top_bottom:b_middle_bottom;

        if(x_middle_bottom>x_top_bottom)
            swap_int(&x_top_bottom, &x_middle_bottom);

        for(int32_t x=x_middle_bottom; x<x_top_bottom; x+=1)
        {
            if(0>x||(int32_t)width<=x) continue;

            pixels[y*width+x]=color;
        }
    }

    for(int32_t y=y1; y<y0; y+=1)
    {
        if(0>y||(int32_t)height<=y) continue;

        int32_t x_top_middle=m_top_middle?(y-b_top_middle)/m_top_middle:b_top_middle;
        int32_t x_top_bottom=m_top_bottom?(y-b_top_bottom)/m_top_bottom:b_top_bottom;

        if(x_top_middle>x_top_bottom)
            swap_int(&x_top_bottom, &x_top_middle);

        for(int32_t x=x_top_middle; x<x_top_bottom; x+=1)
        {
            if(0>x||(int32_t)width<=x) continue;

            pixels[y*width+x]=color;
        }
    }
}

void cherry_fill_circle(uint32_t *pixels, size_t width, size_t height,
                        int32_t cx, int32_t cy, size_t r, uint32_t color)
{
    for(int32_t y=cy-r; y<cy+(int32_t)r; y+=1)
    {
        if(0>y||(int32_t)height<=y) continue;

        for(int32_t x=cx-r; x<cx+(int32_t)r; x+=1)
        {
            if(0>x||(int32_t)width<=x) continue;

            int32_t px=x-cx;
            int32_t py=y-cy;

            if(py*py+px*px<(int32_t)(r*r))
            {
                pixels[y*(int32_t)width+x] = color;
            }
        }
    }
}

#define return_defer(v) do { code=(v); goto defer; } while(0)
Errno cherry_save_to_ppm(uint32_t *pixels, size_t width, size_t height, const char *file_name)
{
    Errno code = 0;
    FILE *f = NULL;
    f = fopen(file_name, "wb");
    if(f == NULL) return_defer(errno);

    {
        fprintf(f, "P6\n%zu %zu\n255\n", width, height);
        if(ferror(f)) return_defer(errno);

        for(size_t i=0; i < width*height; i+=1)
        {
            // 0xAABBGGRR
            uint32_t color = pixels[i];
            uint8_t bytes[3] = {
                (color>>(8*0))&0xFF,
                (color>>(8*1))&0xFF,
                (color>>(8*2))&0xFF
            };
            fwrite(bytes, sizeof(bytes), 1, f);
            if(ferror(f)) return_defer(errno);
        }
    }

defer:
    if(f) fclose(f);
    return code;
}

// Write bmp file headers
Errno generate_bmp_header(uint32_t raw_data, uint8_t color_mode, FILE *f)
{
    uint32_t offset=54;
    if(COLOR_MODE_BGRA==color_mode||COLOR_MODE_RGBA==color_mode)
    {
        offset=122;
    }

    uint32_t file_size = offset + raw_data;
    uint8_t bytes[14] = {
        'B', 'M',
        (uint8_t) file_size,
        (uint8_t) (file_size>>8),
        (uint8_t) (file_size>>16),
        (uint8_t) (file_size>>24),
        0x00, 0x00, 0x00, 0x00,
        (uint8_t) offset,
        (uint8_t) (offset>>8),
        (uint8_t) (offset>>16),
        (uint8_t) (offset>>24)
    };

    fwrite(bytes, sizeof(bytes), 1, f);
    if(ferror(f)) return errno;

    return 0;
}

// Write dib header information into the bmp file
Errno generate_dib_header(uint8_t color_mode, size_t width, size_t height, uint32_t raw_data, FILE *f)
{
    const uint32_t DPI=2835;

    uint32_t header_size;
    uint32_t compression_method;

    uint16_t bits;

    uint32_t alpha_bytes[17]={0};
    if(COLOR_MODE_RGB==color_mode||COLOR_MODE_BGR==color_mode)
    {
        header_size=40;
        bits=24;
        compression_method=0;
    }else
    {
        header_size=108;
        bits=32;
        compression_method=3;

        alpha_bytes[0]=0x00FF0000;
        alpha_bytes[1]=0x0000FF00;
        alpha_bytes[2]=0x000000FF;
        alpha_bytes[3]=0xFF000000;
        // Color Space
        alpha_bytes[4]=0x206E6957;
    }

    uint8_t shared_bytes[] = {
        (uint8_t) header_size, 0, 0, 0,
        (uint8_t) width,
        (uint8_t) (width>>8),
        (uint8_t) (width>>16),
        (uint8_t) (width>>24),
        (uint8_t) height,
        (uint8_t) (height>>8),
        (uint8_t) (height>>16),
        (uint8_t) (height>>24),
        0x01, 0x00,
        (uint8_t) bits,
        (uint8_t) (bits>>8),
        (uint8_t) compression_method,
        (uint8_t) (compression_method>>8), 
        (uint8_t) (compression_method>>16),
        (uint8_t) (compression_method>>24),
        (uint8_t) raw_data,
        (uint8_t) (raw_data>>8),
        (uint8_t) (raw_data>>16),
        (uint8_t) (raw_data>>24),
        (uint8_t) DPI,
        (uint8_t) (DPI>>8),
        (uint8_t) (DPI>>16),
        (uint8_t) (DPI>>24),
        (uint8_t) DPI,
        (uint8_t) (DPI>>8),
        (uint8_t) (DPI>>16),
        (uint8_t) (DPI>>24),
        0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00
    };

    fwrite(shared_bytes, sizeof(shared_bytes), 1, f);
    if(ferror(f)) return errno;

    if(COLOR_MODE_RGBA==color_mode||COLOR_MODE_BGRA==color_mode)
    {
        fwrite(alpha_bytes, sizeof(alpha_bytes), 1, f);
        if(ferror(f)) return errno;
    }

    return 0;
}

// Write image data into the bmp file
Errno create_bitmap_data(uint32_t *pixels, size_t width, size_t height, uint8_t color_mode, FILE *f)
{
    int32_t bytes_size=width*height;
    uint8_t color_size=3;

    uint8_t R=0, G=1, B=2;

    // Flip R and B index
    if(COLOR_MODE_BGRA==color_mode||COLOR_MODE_BGR==color_mode)
    {
        R=2;B=0;
    }

    if(COLOR_MODE_RGB==color_mode||COLOR_MODE_BGR==color_mode)
    {
        // Add a padding of size (width%4)*height at the end of each row
        bytes_size=bytes_size*color_size+(width%4)*height;
    }else
    {
        color_size=4;
        bytes_size*=color_size;
    }

    size_t i, offset=0;

    uint8_t *bytes=(uint8_t*)malloc(bytes_size);

    int32_t byte_index=0;

    for(int32_t y=height-1; y>=0; y-=1)
    for(int32_t x=0; x<(int32_t)width; x+=1)
    {
        i=y*width+x;

        uint32_t color = pixels[i];

        bytes[byte_index*color_size+0+offset]=(color>>(8*B))&0xFF;
        bytes[byte_index*color_size+1+offset]=(color>>(8*G))&0xFF;
        bytes[byte_index*color_size+2+offset]=(color>>(8*R))&0xFF;

        if(COLOR_MODE_BGRA==color_mode||COLOR_MODE_RGBA==color_mode)
        {
            bytes[byte_index*color_size+3]=(color>>(8*3))&0xFF;
        }else if((i+1)%width==0)
        {
            offset+=width%4;
        }

        byte_index++;
    } 

    fwrite(bytes, bytes_size, 1, f);
    free(bytes);

    if(ferror(f)) return errno;

    return 0;
}


// out R
#define calculate_raw_data(w, h, b, R) do { R=b*w*h+(w%4)*h; } while(0);

// Ref: https://en.wikipedia.org/wiki/BMP_file_format
Errno cherry_save_to_bmp(uint32_t *pixels, uint8_t color_mode, size_t width, size_t height, const char *file_name)
{
    Errno code=0;
    FILE *f=NULL;
    f=fopen(file_name, "wb");
    if(f==NULL) return_defer(errno);

    uint32_t raw_data=0;

    {
        switch (color_mode)
        {
        case COLOR_MODE_RGB:
        case COLOR_MODE_BGR:
            calculate_raw_data(width, height, 3, raw_data);
            break;
        case COLOR_MODE_BGRA:
        case COLOR_MODE_RGBA:
            calculate_raw_data(width, height, 4, raw_data);
            break;
        default:
            fprintf(stderr, "Could not save bmp file: Unknown color mode");
        }

        Errno gbh_err = generate_bmp_header(raw_data, color_mode, f);
        if(gbh_err) return_defer(gbh_err);

        Errno gdh_err = generate_dib_header(color_mode, width, height, raw_data, f);
        if(gdh_err) return_defer(gdh_err);

        Errno cbd_err = create_bitmap_data(pixels, width, height, color_mode, f);
        if(cbd_err) return_defer(cbd_err);
    }

defer:
    if(f) fclose(f);
    return code;
}

#endif /* CHERRY_C_ */