#include <glib.h>
#include <stdio.h>

static gint blocksize = 8;
static gdouble quantizer = 16;
static gchar *framesize;
static guint framewidth = 0, frameheight = 0;
static gboolean grayonly = FALSE;
static GOptionEntry entries[] = {
    {"blocksize", 'b', 0, G_OPTION_ARG_INT, &blocksize, "DCT block size [default: 8]", "BLOCKSIZE"},
    {"quantizer", 'q', 0, G_OPTION_ARG_DOUBLE, &quantizer, "Quantizer step length [default: 16.0]", "QUANTIZER"},
    {"size", 's', 0, G_OPTION_ARG_STRING, &framesize, "Frame size, must be multipliers of block size", "WIDTHxHEIGHT"},
    {"gray", 'g', 0, G_OPTION_ARG_NONE, &grayonly, "Use luminance only, grayscale is used instead of yuv420p", NULL},
    {NULL}
};

int lvdodispatch(FILE *fi, FILE *fo, unsigned int blocksize, double quantizer, unsigned int width, unsigned int height, int grayonly);

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
    if(quantizer <= 0.0) {
        g_printerr("Argument error: blocksize should be a positive real number\n");
    }
    if(framesize && framesize[0]) {
        sscanf(framesize, "%ux%u", &framewidth, &frameheight);
    }
    if(framewidth == 0 || frameheight == 0) {
        g_printerr("Argument error: frame size should be specified in WIDTHxHEIGHT\n");
        return 1;
    }
    if(framewidth % blocksize != 0 || frameheight % blocksize != 0) {
        g_printerr("Argument error: frame size can not be divided by %d", blocksize*2);
    }
    return lvdodispatch(stdin, stdout, blocksize, quantizer, framewidth, frameheight, grayonly);
}
