#include <glib.h>

unsigned char prevent_char_overflow(int x) {
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
