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

#ifndef _LVDOCOMMON_H
#define _LVDOCOMMON_H

#include <glib.h>

static inline unsigned char prevent_byte_overflow(int x) {
    if(x > 255) {
        g_printerr("lvdo: [warning] clipping %d to 255\n", x);
        return 255;
    }
    if(x < 0) {
        g_printerr("lvdo: [warning] clipping %d to 0\n", x);
        return 0;
    }
    return x;
}

static inline unsigned char prevent_yuv_overflow(int x) {
    if(x > 235) {
        g_printerr("lvdo: [warning] clipping %d to 235\n", x);
        return 235;
    }
    if(x < 16) {
        g_printerr("lvdo: [warning] clipping %d to 16\n", x);
        return 16;
    }
    return x;
}

/* Adapted from http://rosettacode.org/wiki/Zig-zag_matrix#C
   Licensed under GFDL 1.2 */
static inline unsigned int *new_zigzag_index(unsigned int size) {
    unsigned int *zigzag_index = g_malloc_n(size*size, sizeof (unsigned int));
    unsigned int i, j, idx;
    if(size == 0)
        return zigzag_index;
    for (i = idx = 0; i < size * 2; i++)
        for (j = (i < size) ? 0 : i-size+1; j <= i && j < size; j++)
            zigzag_index[(i&1) ? j*(size-1)+i : (i-j)*size+j ] = idx++;
    return zigzag_index;
}

static inline unsigned int *new_zigzag_reverse(unsigned int size) {
    unsigned int *zigzag_index = new_zigzag_index(size);
    unsigned int *zigzag_reverse = g_malloc_n(size*size, sizeof (unsigned int));
    unsigned int i;
    if(size == 0)
        return zigzag_index;
    for(i = 0; i < size*size; i++)
        zigzag_reverse[zigzag_index[i]] = i;
    g_free(zigzag_index);
    return zigzag_reverse;
}

/*
static unsigned int floor_sqrt(unsigned int x) __attribute__((__const__));
static unsigned int floor_sqrt(unsigned int x) {
    unsigned int sq = 0;
    unsigned int k = 0;
    while(sq < x)
        sq += ++k<<1 | 1;
    return k;
}
*/

static unsigned int ceil_sqrt(unsigned int x) __attribute__((__const__));
static unsigned int ceil_sqrt(unsigned int x) {
    unsigned int sq = 0;
    unsigned int k = 0;
    while(sq < x)
        sq += k++<<1 | 1;
    return k;
}

static inline void print_block(const unsigned int *block, unsigned int size) {
    unsigned int i, j;
    for(i = 0; i < size; i++) {
        for(j = 0; j < size; j++)
            g_printerr("%d ", block[i*size+j]);
        g_printerr(";\n");
    }
    g_printerr("\n");
}

static inline void print_block_double(const double *block, unsigned int size) {
    unsigned int i, j;
    for(i = 0; i < size; i++) {
        for(j = 0; j < size; j++)
            g_printerr("%lf ", block[i*size+j]);
        g_printerr(";\n");
    }
    g_printerr("\n");
}

#endif
