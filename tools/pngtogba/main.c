#include "Image.h"
#include "PaletteOptimiser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>

uint16_t rgb15(struct Colour c)
{
    return ((c.r >> 3) & 31) | (((c.g >> 3) & 31) << 5) | (((c.b >> 3) & 31) << 10);
}

// Returns -1 on an invalid number
static int parseTileSize(const char *tileSizeString)
{
    char *end;
    long int tileSize = strtol(tileSizeString, &end, 10);
    if (*end != '\0' || tileSize <= 0 || tileSize >= INT32_MAX)
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
    if (tileSize == -1 || tileSize % 8 != 0)
    {
        fprintf(stderr, "Invalid tile size %s\n", argv[2]);
        return 1;
    }

    char *filename = argv[1];
    struct Image *img = Image_New(filename);
    struct PaletteOptimiser *optimiser = NULL;

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

    optimiser = PaletteOptimiser_New(tilesX * tilesY);
    assert(optimiser);

    for (int y = 0; y < tilesY; y++)
    {
        for (int x = 0; x < tilesX; x++)
        {
            struct Palette16 *palette = Palette16_New();
            assert(palette);

            for (int j = 0; j < tileSize; j++)
            {
                for (int i = 0; i < tileSize; i++)
                {
                    struct Colour c = Image_Colour(img, x * tileSize + i, y * tileSize + j);
                    uint16_t colour = rgb15(c);

                    if (Palette16_AddColour(palette, colour) == PALETTE16_NUM_COLOURS)
                    {
                        fprintf(stderr, "Tile %d, %d contains more than %d colours!", x, y, PALETTE16_NUM_COLOURS);
                        statusCode = 1;
                        goto exit;
                    }
                }
            }

            int err = PaletteOptimiser_AddPalette(optimiser, palette);
            if (err)
            {
                fprintf(stderr, "Image contains more than 256 colours!");
                statusCode = 1;
                goto exit;
            }
        }
    }

    struct PaletteOptimisationResults results = PaletteOptimiser_OptimisePalettes(optimiser);
    printf("Needed %d palettes\n", results.nPalettes);

    for (int i = 0; i < tilesX * tilesY; i++)
    {
        printf("Tile %d needs palette %d\n", i, results.paletteAssignment[i]);
    }

exit:
    Image_Free(img);
    PaletteOptimiser_Free(optimiser);
    return statusCode;
}