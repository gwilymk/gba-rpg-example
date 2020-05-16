#include "Image.h"
#include "PaletteOptimiser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>

static void printPalette(FILE *file, struct Palette16 *palette, uint16_t transparent);
static int transparentPaletteIndex(struct Palette16 *palette, uint16_t colour, uint16_t transparent);
static void printResults(FILE *file, struct Image *img, struct PaletteOptimisationResults results, uint16_t transparent, int tilesX, int tilesY, int tileSize);
static struct PaletteOptimiser *optimiserForImage(struct Image *img, int tileSize, int tilesX, int tilesY);

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

// Returns INVALID_COLOUR if colour is invalid
uint16_t parseColour(const char *colourString)
{
    char *end;
    long int colourLong = strtol(colourString, &end, 16);
    if (*end != '\0' || colourLong < 0)
    {
        return INVALID_COLOUR;
    }

    struct Colour c = {
        .r = (colourLong >> 16) & 255,
        .g = (colourLong >> 8) & 255,
        .b = (colourLong >> 0) & 255};

    printf("transparent colour: %02x%02x%02x\n", c.r, c.g, c.b);

    return rgb15(c);
}

int main(int argc, char **argv)
{
    int statusCode = 0;
    if (argc <= 2 || argc >= 5)
    {
        fprintf(stderr, "Expected 2 or 3 arguments, usage:\n%s pngfile tilesize transparentColour\n", argv[0]);
        return 1;
    }

    int tileSize = parseTileSize(argv[2]);
    if (tileSize == -1 || tileSize % 8 != 0)
    {
        fprintf(stderr, "Invalid tile size %s\n", argv[2]);
        return 1;
    }

    uint16_t transparent = INVALID_COLOUR;
    if (argc == 4)
    {
        transparent = parseColour(argv[3]);
        if (transparent == INVALID_COLOUR)
        {
            fprintf(stderr, "Invalid transparent colour %s\n", argv[3]);
            return 1;
        }
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

    optimiser = optimiserForImage(img, tileSize, tilesX, tilesY);
    struct PaletteOptimisationResults results = PaletteOptimiser_OptimisePalettes(optimiser, transparent);

    if (results.nPalettes == 0)
    {
        fprintf(stderr, "Failed to find a set of covering palettes");
        statusCode = 1;
        goto exit;
    }

    printResults(stdout, img, results, transparent, tilesX, tilesY, tileSize);

exit:
    Image_Free(img);
    PaletteOptimiser_Free(optimiser);
    return statusCode;
}

static struct PaletteOptimiser *optimiserForImage(struct Image *img, int tileSize, int tilesX, int tilesY)
{
    struct PaletteOptimiser *optimiser = PaletteOptimiser_New(tilesX * tilesY);
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
                        exit(1);
                    }
                }
            }

            int err = PaletteOptimiser_AddPalette(optimiser, palette);
            if (err)
            {
                fprintf(stderr, "Image contains more than 256 colours!");
                exit(1);
            }
        }
    }

    return optimiser;
}

static void printResults(FILE *file, struct Image *img, struct PaletteOptimisationResults results, uint16_t transparent, int tilesX, int tilesY, int tileSize)
{
    fprintf(file, "#include <stdint.h>\n\n");
    fprintf(file, "uint16_t paletteData[256] = {\n");

    for (int i = 0; i < results.nPalettes; i++)
    {
        struct Palette16 *palette = results.palettes[i];
        printPalette(file, palette, transparent);
    }

    for (int i = results.nPalettes; i < 16; i++)
    {
        fprintf(file, "    ");
        for (int j = 0; j < 16; j++)
        {
            fprintf(file, "0x0000, ");
        }
        fprintf(file, "\n");
    }

    fprintf(file, "};\n\n");

    fprintf(file, "uint32_t tileData = {\n");
    int tileDataLength = 0;

    for (int y = 0; y < tilesY; y++)
    {
        for (int x = 0; x < tilesX; x++)
        {
            int paletteIndex = results.paletteAssignment[y * tilesX + x];
            struct Palette16 *palette = results.palettes[paletteIndex];

            fprintf(file, "    /* %d, %d (palette index %d) */\n", x, y, paletteIndex);

            for (int innerY = 0; innerY < tileSize / 8; innerY++)
            {
                fprintf(file, "    ");

                for (int innerX = 0; innerX < tileSize / 8; innerX++)
                {
                    for (int j = innerY; j < innerY + 8; j++)
                    {
                        fprintf(file, "0x");

                        for (int i = innerX; i < innerX + 8; i++)
                        {
                            struct Colour c = Image_Colour(img, x * tileSize + i, y * tileSize + j);
                            uint16_t colour = rgb15(c);

                            int colourIndex = transparentPaletteIndex(palette, colour, transparent);
                            fprintf(file, "%x", colourIndex);
                        }

                        fprintf(file, ", ");
                        tileDataLength += 4;
                    }
                }

                fprintf(file, "\n");
            }
        }
    }

    fprintf(file, "};\n\n");
    fprintf(file, "int tileDataLength = %d;\n\n", tileDataLength);

    fprintf(file, "int tilePaletteNumber = {");
    for (int i = 0; i < tilesX * tilesY; i++)
    {
        if (i % 16 == 0)
        {
            fprintf(file, "\n   ");
        }

        fprintf(file, "%d, ", results.paletteAssignment[i]);
    }
    fprintf(file, "\n};\n");
}

static int transparentPaletteIndex(struct Palette16 *palette, uint16_t colour, uint16_t transparent)
{
    if (colour == transparent)
    {
        return 0;
    }

    return Palette16_GetIndex(palette, colour) + 1;
}

static void printPalette(FILE *file, struct Palette16 *palette, uint16_t transparent)
{
    fprintf(file, "    ");

    if (transparent != INVALID_COLOUR)
    {
        fprintf(file, "0x%04x, ", transparent);
    }

    for (int j = 0; j < Palette16_GetNumColours(palette); j++)
    {
        uint16_t colour = Palette16_GetColour(palette, j);
        if (colour == transparent)
        {
            continue;
        }

        fprintf(file, "0x%04x, ", colour);
    }

    for (int j = Palette16_GetNumColours(palette); j < PALETTE16_NUM_COLOURS; j++)
    {
        fprintf(file, "0x0000, ");
    }

    fprintf(file, "\n");
}