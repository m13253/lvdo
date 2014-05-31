#ifndef _LVDOCOMMON_H
#define _LVDOCOMMON_H

unsigned char prevent_byte_overflow(int x);

unsigned char prevent_yuv_overflow(int x);

unsigned int *new_zigzag_index(unsigned int size);

unsigned int *new_zigzag_reverse(unsigned int size);

void print_block(const unsigned int *block, unsigned int size);

void print_block_double(const double *block, unsigned int size);

#endif
