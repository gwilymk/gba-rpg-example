#define _GNU_SOURCE // cause stdio.h to include asprintf

#include "Image.h"

#include "spng/spng.h"
#include <stdlib.h>
#include <stdio.h>

struct Image
{
    size_t bufferLen;
    unsigned char *buffer;

    char *error;
};

struct Image *Image_New(const char *filename)
{
    struct Image *img = calloc(1, sizeof(struct Image));
    char *pngBuffer = NULL;

    if (img == NULL)
    {
        goto error;
    }

    FILE *png = fopen(filename, "rb");
    if (png == NULL)
    {
        asprintf(&img->error, "Failed to open file %s", filename);
        goto error;
    }

    fseek(png, 0, SEEK_END);
    long pngBufferSize = ftell(png);
    rewind(png);

    if (pngBufferSize < 1)
    {
        asprintf(&img->error, "Loading png file %s failed", filename);
        goto error;
    }

    pngBuffer = malloc(pngBufferSize);
    if (pngBuffer == NULL)
    {
        asprintf(&img->error, "Failed to allocate buffer for png file %s", filename);
        goto error;
    }

    if (fread(pngBuffer, pngBufferSize, 1, png) != 1)
    {
        asprintf(&img->error, "Failed to read png file to buffer %s", filename);
        goto error;
    }

    fclose(png);
    png = NULL;

    spng_ctx *ctx = spng_ctx_new(0);
    if (ctx == NULL)
    {
        asprintf(&img->error, "Failed to create png context");
        goto error;
    }

    int err = spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);
    if (err)
    {
        asprintf(&img->error, "Failed to set crc action: %s", spng_strerror(err));
        goto error;
    }

    err = spng_set_png_buffer(ctx, pngBuffer, pngBufferSize);
    if (err)
    {
        asprintf(&img->error, "Failed to set png buffer: %s", spng_strerror(err));
        goto error;
    }

    err = spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &img->bufferLen);
    if (err)
    {
        asprintf(&img->error, "Failed to get decoded image size: %s", spng_strerror(err));
        goto error;
    }

    img->buffer = malloc(img->bufferLen);
    if (img->buffer == NULL)
    {
        asprintf(&img->error, "Failed to allocate buffer for decoded image data");
        goto error;
    }

    err = spng_decode_image(ctx, img->buffer, img->bufferLen, SPNG_FMT_RGBA8, 0);
    if (err)
    {
        asprintf(&img->error, "Failed to decode image: %s", spng_strerror(err));
        goto error;
    }

    spng_ctx_free(ctx);
    free(pngBuffer);

    return img;

error:
    if (png != NULL)
    {
        fclose(png);
    }

    spng_ctx_free(ctx);

    free(pngBuffer);
    free(img->buffer);
    return img;
}

char *Image_Error(struct Image *img)
{
    return img->error;
}

void Image_Free(struct Image *img)
{
    free(img->error);
    free(img->buffer);
    free(img);
}