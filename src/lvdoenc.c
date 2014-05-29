#include <fftw3.h>
#include <glib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "lvdocommon.h"

static void lvdo_enc_frame(const unsigned char *payload, unsigned char *frame, unsigned int blocksize, unsigned int quantizer, unsigned int qmin, unsigned int qmax, unsigned int width, unsigned int height, double *in, double *out, const fftw_plan plan, const unsigned int *zigzag_reverse);

int lvdo_dispatch(FILE *fi, FILE *fo, unsigned int blocksize, unsigned int quantizer, unsigned int qmin, unsigned int qmax, unsigned int width, unsigned int height, int grayonly) {
    size_t payloadleny = width*height*(qmax-qmin)*(8-quantizer)/(blocksize*blocksize*8);
    size_t payloadlen = grayonly ? payloadleny : payloadleny*3/2;
    unsigned char *payload = g_malloc(payloadlen+1);
    unsigned char *framey = g_malloc(width*height);
    unsigned char *frameuv = g_malloc(width*height/2);
    unsigned int *zigzag_reverse = new_zigzag_reverse(blocksize);
    double *in = fftw_malloc(blocksize*blocksize*sizeof (double));
    double *out = fftw_malloc(blocksize*blocksize*sizeof (double));
    fftw_plan plan = fftw_plan_r2r_2d(blocksize, blocksize, in, out, FFTW_REDFT01, FFTW_REDFT01, FFTW_PATIENT | FFTW_DESTROY_INPUT);
    memset(in, 0, blocksize*blocksize*sizeof (double));
    if(grayonly)
        memset(frameuv, 0x80, width*height/2);
    if(payloadlen != 0)
        g_printerr("lvdo: [info] bytes per frame: %lu\n", (unsigned long) payloadlen);
    else {
        g_printerr("lvdo: [error] bytes per frame: 0\n");
        return 1;
    }
    while(!feof(fi)) {
        int readres = fread(payload, 1, payloadlen, fi);
        if(readres == 0)
            break;
        memset(payload+readres, 0, payloadlen+1-readres);
        lvdo_enc_frame(payload, framey, blocksize, quantizer, qmin, qmax, width, height, in, out, plan, zigzag_reverse);
        fwrite(framey, 1, width*height, fo);
        if(!grayonly)
            lvdo_enc_frame(payload+payloadleny, frameuv, blocksize, quantizer, qmin, qmax, width/2, height, in, out, plan, zigzag_reverse);
        fwrite(frameuv, 1, width*height/2, fo);
    }
    fftw_free(in); fftw_free(out); fftw_destroy_plan(plan);
    g_free(payload); g_free(framey); g_free(frameuv); g_free(zigzag_reverse);
    return 0;
}

static void lvdo_enc_frame(const unsigned char *payload, unsigned char *frame, unsigned int blocksize, unsigned int quantizer, unsigned int qmin, unsigned int qmax, unsigned int width, unsigned int height, double *in, double *out, const fftw_plan plan, const unsigned int *zigzag_reverse) {
    unsigned int payloadi = 0, blocki, blockj, pixeli, pixelj;
    unsigned int lastbyte = 0, availbit = 0;
    for(blocki = 0; blocki*blocksize < height; blocki++)
        for(blockj = 0; blockj*blocksize < width; blockj++) {
            memset(in, 0, blocksize*blocksize*sizeof (double));
            for(pixeli = qmin; pixeli < qmax; pixeli++) {
                if(availbit == 0) {
                    lastbyte = payload[payloadi++];
                    availbit = 8;
                } else if(availbit < 8-quantizer) {
                    lastbyte |= ((unsigned int) payload[payloadi++])<<availbit;
                    availbit += 8;
                }
                in[zigzag_reverse[pixeli]] = (signed char) ((lastbyte & 0xff>>quantizer)<<quantizer ^ 0x80)*0.84375/(blocksize*2*ceil(sqrt(qmax-qmin)));
                lastbyte >>= 8-quantizer;
                availbit -= 8-quantizer;
            }
            in[0] += 64.0; // Shift output by +128
            print_block_double(in, blocksize);
            in[0] *= 2; // Normalize for FFTW
            for(pixeli = 1; pixeli < blocksize; pixeli++)
                in[pixeli] *= M_SQRT2;
            for(pixeli = blocksize; pixeli < blocksize*blocksize; pixeli += blocksize)
                in[pixeli] *= M_SQRT2;
            fftw_execute(plan);
            for(pixeli = 0; pixeli < blocksize; pixeli++)
                for(pixelj = 0; pixelj < blocksize; pixelj++)
                    frame[(blocki*blocksize+pixeli)*width+(blockj*blocksize+pixelj)] = prevent_yuv_overflow(round(out[pixeli*blocksize+pixelj]));
        }
}
