#ifndef CHERRY_NOISE_C_
#define CHERRY_NOISE_C_

#ifndef CHERRY_C_
#include <math.h>
#include <stdlib.h>
#include <time.h>
#define PI 3.14159265358979323846

#endif // !CHERRY_C_

const int32_t PERLIN_YWRAPB = 4;
const int32_t PERLIN_YWRAP = 16;
const int32_t PERLIN_ZWRAPB = 8;
const int32_t PERLIN_ZWRAP = 256;
const int32_t PERLIN_SIZE = 4095;


int32_t perlin_octaves = 4; // default to medium smooth
float perlin_amp_falloff = 0.5; // 50% reduction/octave

float *perlin = NULL;

float scaled_cosine(float i)
{
    return 0.5*(1.0-cos(i*PI));
}

// Ref [1]: https://mrl.cs.nyu.edu/~perlin/noise/
// Ref [2]: https://github.com/processing/p5.js/blob/e32b45367baad694b1f4eeec0586b910bfcf0724/src/math/noise.js
float noise(float x, float y, float z) {
    srand(time(0));
    if (perlin == NULL) {
        perlin = (float *)malloc((PERLIN_SIZE+1)*sizeof(float));
        for (int32_t i = 0; i < PERLIN_SIZE + 1; i++) {
            perlin[i] = (float) rand() / (float) RAND_MAX; 
        }
    }

    if (x < 0) {
        x = -x;
    }
    if (y < 0) {
        y = -y;
    }
    if (z < 0) {
        z = -z;
    }

    int32_t xi = floor(x),
            yi = floor(y),
            zi = floor(z);

    float xf = x - xi;
    float yf = y - yi;
    float zf = z - zi;
    float rxf, ryf;

    float r = 0;
    float ampl = 0.5;

    float n1, n2, n3;

    for (int32_t o = 0; o < perlin_octaves; o++) {
        int32_t of = xi + (yi << PERLIN_YWRAPB) + (zi << PERLIN_ZWRAPB);

        rxf = scaled_cosine(xf);
        ryf = scaled_cosine(yf);

        n1 = perlin[of & PERLIN_SIZE];
        n1 += rxf * (perlin[(of + 1) & PERLIN_SIZE] - n1);
        n2 = perlin[(of + PERLIN_YWRAP) & PERLIN_SIZE];
        n2 += rxf * (perlin[(of + PERLIN_YWRAP + 1) & PERLIN_SIZE] - n2);
        n1 += ryf * (n2 - n1);

        of += PERLIN_ZWRAP;
        n2 = perlin[of & PERLIN_SIZE];
        n2 += rxf * (perlin[(of + 1) & PERLIN_SIZE] - n2);
        n3 = perlin[(of + PERLIN_YWRAP) & PERLIN_SIZE];
        n3 += rxf * (perlin[(of + PERLIN_YWRAP + 1) & PERLIN_SIZE] - n3);
        n2 += ryf * (n3 - n2);

        n1 += scaled_cosine(zf) * (n2 - n1);

        r += n1 * ampl;
        ampl *= perlin_amp_falloff;
        xi <<= 1;
        xf *= 2;
        yi <<= 1;
        yf *= 2;
        zi <<= 1;
        zf *= 2;

        if (xf >= 1.0) {
            xi++;
            xf--;
        }
        if (yf >= 1.0) {
            yi++;
            yf--;
        }
        if (zf >= 1.0) {
            zi++;
            zf--;
        }
    }
    return r;
};
#endif /* CHERRY_NOISE_C_ */
