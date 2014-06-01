/*
    Copyright (C) 2014  StarBrilliant <m13253@hotmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fftw3.h>
#include <glib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "lvdocommon.h"

static void lvdo_enc_frame(const unsigned char *payload, size_t payloadlen, unsigned char *frame, unsigned int blocksize, unsigned int quantizer, unsigned int qmin, unsigned int qmax, unsigned int width, unsigned int height, int verbose, double *in, double *out, const fftw_plan plan, const unsigned int *zigzag_reverse);

int lvdo_dispatch(FILE *fi, FILE *fo, unsigned int blocksize, unsigned int quantizer, unsigned int qmin, unsigned int qmax, unsigned int width, unsigned int height, int grayonly, int verbose) {
    size_t payloadleny = width*height*(qmax-qmin)*(8-quantizer)/(blocksize*blocksize*8);
    size_t payloadlen = grayonly ? payloadleny : payloadleny*3/2;
    unsigned char *payload = g_malloc(payloadlen);
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
        if(readres < payloadlen)
            memset(payload+readres, 0, payloadlen-readres);
        lvdo_enc_frame(payload, payloadleny, framey, blocksize, quantizer, qmin, qmax, width, height, verbose, in, out, plan, zigzag_reverse);
        fwrite(framey, 1, width*height, fo);
        if(!grayonly)
            lvdo_enc_frame(payload+payloadleny, payloadleny/2, frameuv, blocksize, quantizer, qmin, qmax, width/2, height, verbose, in, out, plan, zigzag_reverse);
        fwrite(frameuv, 1, width*height/2, fo);
    }
    fftw_free(in); fftw_free(out); fftw_destroy_plan(plan);
    g_free(payload); g_free(framey); g_free(frameuv); g_free(zigzag_reverse);
    return 0;
}

static void lvdo_enc_frame(const unsigned char *payload, size_t payloadlen, unsigned char *frame, unsigned int blocksize, unsigned int quantizer, unsigned int qmin, unsigned int qmax, unsigned int width, unsigned int height, int verbose, double *in, double *out, const fftw_plan plan, const unsigned int *zigzag_reverse) {
    unsigned int payloadi = 0, blocki, blockj, pixeli, pixelj;
    unsigned int lastbyte = 0, availbit = 0;
    for(blocki = 0; blocki*blocksize < height; blocki++)
        for(blockj = 0; blockj*blocksize < width; blockj++) {
            memset(in, 0, blocksize*blocksize*sizeof (double));
            for(pixeli = qmin; pixeli < qmax; pixeli++) {
                if(availbit == 0) {
                    if(payloadi != payloadlen)
                        lastbyte = payload[payloadi++];
                    availbit = 8;
                } else if(availbit < 8-quantizer) {
                    if(payloadi != payloadlen)
                        lastbyte |= ((unsigned int) payload[payloadi++])<<availbit;
                    availbit += 8;
                }
                in[zigzag_reverse[pixeli]] = (signed char) ((lastbyte & 0xff>>quantizer)<<quantizer ^ 0x80)*0.84375/(blocksize*2*ceil_sqrt(qmax-qmin));
                lastbyte >>= 8-quantizer;
                availbit -= 8-quantizer;
            }
            in[0] += 64.0; // Shift output by +128
            if(verbose)
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
