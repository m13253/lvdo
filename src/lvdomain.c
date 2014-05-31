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

#include <glib.h>
#include <stdio.h>

static gint blocksize = 8;
static gint quantizer = 4;
static gint qmin = 1;
static gint qmax = -1;
static gchar *framesize;
static guint framewidth = 0, frameheight = 0;
static gboolean grayonly = FALSE;
static gboolean verbose = FALSE;
static GOptionEntry entries[] = {
    {"blocksize", 'b', 0, G_OPTION_ARG_INT, &blocksize, "DCT block size [default: 8]", "BLOCKSIZE"},
    {"quantizer", 'q', 0, G_OPTION_ARG_INT, &quantizer, "Quantizer step length, lower for more data capacity (0..7) [default: 4]", "QUANTIZER"},
    {"qmin", 0, 0, G_OPTION_ARG_INT, &qmin, "Minimum DCT index used, lower for more data capacity (0..BLOCKSIZE*BLOCKSIZE-1) [default: 1]", "QMIN"},
    {"qmax", 0, 0, G_OPTION_ARG_INT, &qmax, "Maximum DCT index used, higher for more data capacity (1..BLOCKSIZE*BLOCKSIZE) [default: BLOCKSIZE*BLOCKSIZE/2]", "QMAX"},
    {"size", 's', 0, G_OPTION_ARG_STRING, &framesize, "Frame size, must be multipliers of block size", "WIDTHxHEIGHT"},
    {"gray", 'g', 0, G_OPTION_ARG_NONE, &grayonly, "Use luminance only", NULL},
    {"verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Print debug information", NULL},
    {NULL}
};

int lvdo_dispatch(FILE *fi, FILE *fo, unsigned int blocksize, unsigned int quantizer, unsigned int qmin, unsigned int qmax, unsigned int width, unsigned int height, int grayonly, int verbose);

int main(int argc, char *argv[]) {
    GError *error = NULL;
    GOptionContext *context;
    
    context = g_option_context_new("- video steganography experiments");
    g_option_context_add_main_entries(context, entries, NULL);
    if(!g_option_context_parse(context, &argc, &argv, &error)) {
        g_printerr("Argument error: %s\n", error->message);
        g_option_context_free(context);
        return 1;
    }
    g_option_context_free(context);
    if(blocksize <= 0) {
        g_printerr("Argument error: blocksize should be a positive integer\n");
        return 1;
    }
    if(quantizer < 0 || quantizer > 7) {
        g_printerr("Argument error: quantizer should be between 0 and 7\n");
        return 1;
    }
    if(qmax == -1)
        qmax = blocksize*blocksize/2;
    if(qmin < 0 || qmin > blocksize*blocksize) {
        g_printerr("Argument error: qmin should be between 0 and %d\n", blocksize*blocksize);
        return 1;
    }
    if(qmax < 0 || qmax > blocksize*blocksize) {
        g_printerr("Argument error: qmax should be between 0 and %d\n", blocksize*blocksize);
        return 1;
    }
    if(qmin >= qmax) {
        g_printerr("Argument error: qmax should be greater than qmin\n");
        return 1;
    }
    if(framesize && framesize[0]) {
        sscanf(framesize, "%ux%u", &framewidth, &frameheight);
    }
    if(framewidth == 0 || frameheight == 0) {
        g_printerr("Argument error: frame size should be specified with \"-s WIDTHxHEIGHT\"\n");
        return 1;
    }
    if(grayonly)
        if(framewidth % blocksize != 0 || frameheight % blocksize != 0) {
            g_printerr("Argument error: frame size can not be divided by %d", blocksize);
            return 1;
        } else;
    else
        if(framewidth % (blocksize*2) != 0 || frameheight % (blocksize*2) != 0) {
            g_printerr("Argument error: frame size can not be divided by %d", blocksize*2);
            return 1;
        }
    return lvdo_dispatch(stdin, stdout, blocksize, quantizer, qmin, qmax, framewidth, frameheight, grayonly, verbose);
}
