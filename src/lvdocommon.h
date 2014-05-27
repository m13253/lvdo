#ifndef _LVDOCOMMON_H
#define _LVDOCOMMON_H

unsigned char prevent_yuv_overflow(int x);

unsigned int *new_zigzag_index(unsigned int size);

unsigned int *new_zigzag_reverse(unsigned int size);

#endif
