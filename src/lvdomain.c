#include <glib.h>
#include <stdio.h>

static gint blocksize = 8;
static gint quantizer = 4;
static gint qmin = 0;
static gint qmax = -1;
static gchar *framesize;
static guint framewidth = 0, frameheight = 0;
static gboolean grayonly = FALSE;
static GOptionEntry entries[] = {
    {"blocksize", 'b', 0, G_OPTION_ARG_INT, &blocksize, "DCT block size [default: 8]", "BLOCKSIZE"},
    {"quantizer", 'q', 0, G_OPTION_ARG_INT, &quantizer, "Quantizer step length [default: 4]", "QUANTIZER"},
    {"qmin", 0, 0, G_OPTION_ARG_INT, &qmin, "Minimum DCT index used [default: 0]", "QMIN"},
    {"qmax", 0, 0, G_OPTION_ARG_INT, &qmax, "Maximum DCT index used [default: QUANTIZER*QUANTIZER]", "QMAX"},
    {"size", 's', 0, G_OPTION_ARG_STRING, &framesize, "Frame size, must be multipliers of block size", "WIDTHxHEIGHT"},
    {"gray", 'g', 0, G_OPTION_ARG_NONE, &grayonly, "Use luminance only", NULL},
    {NULL}
};

int lvdo_dispatch(FILE *fi, FILE *fo, unsigned int blocksize, unsigned int quantizer, unsigned int qmin, unsigned int qmax, unsigned int width, unsigned int height, int grayonly);

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
        qmax = quantizer*quantizer;
    if(qmin < 0 || qmin > quantizer*quantizer) {
        g_printerr("Argument error: qmin should be between 0 and %d\n", quantizer*quantizer);
        return 1;
    }
    if(qmax < 0 || qmax > quantizer*quantizer) {
        g_printerr("Argument error: qmax should be between 0 and %d\n", quantizer*quantizer);
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
    if(framewidth % (blocksize*2) != 0 || frameheight % (blocksize*2) != 0) {
        g_printerr("Argument error: frame size can not be divided by %d", blocksize*2);
        return 1;
    }
    return lvdo_dispatch(stdin, stdout, blocksize, quantizer, qmin, qmax, framewidth, frameheight, grayonly);
}
