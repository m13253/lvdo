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

static void lvdo_dec_frame(unsigned char *payload, size_t payloadlen, const unsigned char *frame, unsigned int blocksize, unsigned int quantizer, unsigned int qmin, unsigned int qmax, unsigned int width, unsigned int height, int verbose, double *in, double *out, const fftw_plan plan, const unsigned int *zigzag_reverse);

int lvdo_dispatch(FILE *fi, FILE *fo, unsigned int blocksize, unsigned int quantizer, unsigned int qmin, unsigned int qmax, unsigned int width, unsigned int height, int grayonly, int verbose) {
    size_t payloadlen = width*height*(qmax-qmin)*(8-quantizer)/(blocksize*blocksize*8);
    unsigned char *payload = g_malloc(payloadlen);
    unsigned char *framey = g_malloc(width*height*3/2);
    unsigned char *frameuv = framey+width*height;
    unsigned int *zigzag_reverse = new_zigzag_reverse(blocksize);
    double *in = fftw_malloc(blocksize*blocksize*sizeof (double));
    double *out = fftw_malloc(blocksize*blocksize*sizeof (double));
    fftw_plan plan = fftw_plan_r2r_2d(blocksize, blocksize, in, out, FFTW_REDFT10, FFTW_REDFT10, FFTW_PATIENT | FFTW_DESTROY_INPUT);
    while(!feof(fi)) {
        int readres = fread(framey, 1, width*height*3/2, fi);
        if(readres == 0)
            break;
        if(readres < width*height*3/2)
            memset(framey+readres, 0, width*height*3/2-readres);
        lvdo_dec_frame(payload, payloadlen, framey, blocksize, quantizer, qmin, qmax, width, height, verbose, in, out, plan, zigzag_reverse);
        fwrite(payload, 1, payloadlen, fo);
        if(!grayonly) {
            lvdo_dec_frame(payload, payloadlen/2, frameuv, blocksize, quantizer, qmin, qmax, width/2, height, verbose, in, out, plan, zigzag_reverse);
            fwrite(payload, 1, payloadlen/2, fo);
        }
    }
    fftw_free(in); fftw_free(out); fftw_destroy_plan(plan);
    g_free(payload); g_free(framey); g_free(zigzag_reverse);
    return 0;
}

static void lvdo_dec_frame(unsigned char *payload, size_t payloadlen, const unsigned char *frame, unsigned int blocksize, unsigned int quantizer, unsigned int qmin, unsigned int qmax, unsigned int width, unsigned int height, int verbose, double *in, double *out, const fftw_plan plan, const unsigned int *zigzag_reverse) {
    unsigned int payloadi = 0, blocki, blockj, pixeli, pixelj;
    unsigned int lastbyte = 0, availbit = 0;
    for(blocki = 0; blocki*blocksize < height; blocki++)
        for(blockj = 0; blockj*blocksize < width; blockj++) {
            for(pixeli = 0; pixeli < blocksize; pixeli++)
                for(pixelj = 0; pixelj < blocksize; pixelj++)
                    in[pixeli*blocksize+pixelj] = (signed char) (frame[(blocki*blocksize+pixeli)*width+(blockj*blocksize+pixelj)] ^ 0x80)*(int) ceil_sqrt(qmax-qmin)/(blocksize*2*0.84375);
            fftw_execute(plan);
            out[0] /= 2;
            for(pixeli = 1; pixeli < blocksize; pixeli++)
                out[pixeli] *= M_SQRT1_2;
            for(pixeli = blocksize; pixeli < blocksize*blocksize; pixeli += blocksize)
                out[pixeli] *= M_SQRT1_2;
            if(verbose)
                print_block_double(out, blocksize);
            for(pixeli = qmin; pixeli < qmax; pixeli++) {
                lastbyte |= prevent_byte_overflow(round((out[zigzag_reverse[pixeli]]+128)/(1<<quantizer)))<<availbit;
                availbit += 8-quantizer;
                if(availbit > 7) {
                    if(payloadi != payloadlen)
                        payload[payloadi++] = (unsigned char) lastbyte;
                    lastbyte >>= 8;
                    availbit -= 8;
                }
            }
        }
}
