#include <fftw3.h>
#include <glib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static unsigned char prevent_char_overflow(int x);

int lvdo_dispatch(FILE *fi, FILE *fo, unsigned int blocksize, unsigned int quantizer, unsigned int width, unsigned int height, int grayonly) {
    size_t payloadlen = (grayonly ? width*height : width*height*3/2)*(8-quantizer)/8;
    unsigned char *payload = g_malloc(payloadlen);
    unsigned char *framey = g_malloc(width*height);
    unsigned char *frameuv = g_malloc(width*height/2);
    double *in = fftw_malloc(blocksize*blocksize*sizeof (double));
    double *out = fftw_malloc(blocksize*blocksize*sizeof (double));
    fftw_plan plan = fftw_plan_r2r_2d(blocksize, blocksize, in, out, FFTW_REDFT01, FFTW_REDFT01, FFTW_PATIENT | FFTW_DESTROY_INPUT);
    if(grayonly)
        memset(frameuv, 0x80, width*height/2);
    while(!feof(fi)) {
        int readres = fread(payload, 1, payloadlen, fi);
        unsigned int payloadi = 0, blocki, blockj, pixeli, pixelj;
        unsigned int lastbyte = 0, availbit = 0;
        if(readres == 0)
            break;
        if(readres < payloadlen)
            memset(payload+readres, 0, payloadlen-readres);
        for(blocki = 0; blocki*blocksize < (grayonly ? height : height*3/2); blocki++)
            for(blockj = 0; blockj*blocksize < width; blockj++) {
                for(pixeli = 0; pixeli < blocksize*blocksize; pixeli++) {
                    if(availbit == 0) {
                        lastbyte = payload[payloadi++];
                        availbit = 8;
                    } else if(availbit < 8-quantizer) {
                        lastbyte |= ((unsigned int) payload[payloadi++])<<availbit;
                        availbit += 8;
                    }
                    in[pixeli] = ((int) ((lastbyte&(0xff>>quantizer))<<quantizer)-128)/(blocksize*2.0);
                    lastbyte >>= (8-quantizer);
                    availbit -= 8-quantizer;
                }
                in[0] *= 2; // Normalize for FFTW
                for(pixeli = 1; pixeli < blocksize; pixeli++)
                    in[pixeli] *= M_SQRT2;
                for(pixeli = blocksize; pixeli < blocksize*blocksize; pixeli += blocksize)
                    in[pixeli] *= M_SQRT2;
                fftw_execute(plan);
                for(pixeli = 0; pixeli < blocksize; pixeli++)
                    for(pixelj = 0; pixelj < blocksize; pixelj++)
                        if(blocki*blocksize < height)
                            framey[(blocki*blocksize+pixeli)*width+(blockj*blocksize+pixelj)] = prevent_char_overflow(round(out[pixeli*blocksize+pixelj]/blocksize)+128);
                        else
                            frameuv[(blocki*blocksize+pixeli-height)*width/2+(blockj*blocksize+pixelj)] = prevent_char_overflow(round(out[pixeli*blocksize+pixelj]/blocksize)+128);
            }
        fwrite(framey, 1, width*height, fo);
        fwrite(frameuv, 1, width*height/2, fo);
    }
    fftw_free(in);
    fftw_free(out);
    fftw_destroy_plan(plan);
    return 0;
}

static unsigned char prevent_char_overflow(int x) {
    if(x > 255) {
        g_warning("Clipping %d to 255", x);
        return 255;
    }
    if(x < 0) {
        g_warning("Clipping %d to 0", x);
        return 0;
    }
    return x;
}
