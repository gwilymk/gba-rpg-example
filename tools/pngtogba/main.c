#include "Image.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

// Returns -1 on an invalid number
static int parseTileSize(const char *tileSizeString)
{
    char *end;
    long int tileSize = strtol(tileSizeString, &end, 10);
    if (*end != '\0' || tileSize <= 0 || tileSize >= INT_MAX)
    {
        return -1;
    }

    return tileSize;
}

int main(int argc, char **argv)
{
    int statusCode = 0;
    if (argc != 3)
    {
        fprintf(stderr, "Expected exactly 2 arguments, usage:\n%s pngfile tilesize\n", argv[0]);
        return 1;
    }

    int tileSize = parseTileSize(argv[2]);
    if (tileSize == -1)
    {
        fprintf(stderr, "Invalid tile size %s\n", argv[2]);
        return 1;
    }

    char *filename = argv[1];
    struct Image *img = Image_New(filename);

    char *error = NULL;
    if (img == NULL || (error = Image_Error(img)) != NULL)
    {
        fprintf(stderr, "Failed to load image\n");
        if (error)
        {
            fprintf(stderr, "Error: %s\n", error);
        }

        return 1;
    }

    int width = Image_Width(img);
    int height = Image_Height(img);

    if (width % tileSize != 0 || height % tileSize != 0)
    {
        fprintf(stderr, "Image width or height not a multiple of the tile size\n");
        statusCode = 1;
        goto exit;
    }

    int tilesX = width / tileSize;
    int tilesY = height / tileSize;

    printf("%dx%d\n", tilesX, tilesY);

exit:
    Image_Free(img);
    return statusCode;
}