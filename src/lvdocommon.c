#include <glib.h>

unsigned char prevent_yuv_overflow(int x) {
    if(x > 235) {
        g_warning("Clipping %d to 235", x);
        return 235;
    }
    if(x < 16) {
        g_warning("Clipping %d to 16", x);
        return 16;
    }
    return x;
}

/* Adapted from http://rosettacode.org/wiki/Zig-zag_matrix#C
   Licensed under GFDL 1.2 */
unsigned int *new_zigzag_index(unsigned int size) {
    unsigned int *zigzag_index = g_malloc_n(size*size, sizeof (unsigned int));
    unsigned int i, j, idx;
    if(size == 0)
        return zigzag_index;
    for (i = idx = 0; i < size * 2; i++)
        for (j = (i < size) ? 0 : i-size+1; j <= i && j < size; j++)
            zigzag_index[(i&1) ? j*(size-1)+i : (i-j)*size+j ] = idx++;
    return zigzag_index;
}

unsigned int *new_zigzag_reverse(unsigned int size) {
    unsigned int *zigzag_index = new_zigzag_index(size);
    unsigned int *zigzag_reverse = g_malloc_n(size*size, sizeof (unsigned int));
    unsigned int i;
    if(size == 0)
        return zigzag_index;
    for(i = 0; i<size*size; i++)
        zigzag_reverse[zigzag_index[i]] = i;
    g_free(zigzag_index);
    return zigzag_reverse;
}

void print_block(const int *block, unsigned int size) {
    unsigned int i, j;
    for(i = 0; i < size; i++) {
        for(j = 0; j < size; j++)
            g_printerr("%d ", block[i*size+j]);
        g_printerr("\n");
    }
    g_printerr("\n");
}

void print_block_double(const double *block, unsigned int size) {
    unsigned int i, j;
    for(i = 0; i < size; i++) {
        for(j = 0; j < size; j++)
            g_printerr("%lf ", block[i*size+j]);
        g_printerr("\n");
    }
    g_printerr("\n");
}
