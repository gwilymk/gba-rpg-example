#include "Image.h"
#include "PaletteOptimiser.h"
#include "ConfigReader.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

static void printPalette(FILE *file, struct Palette16 *palette, uint16_t transparent);
static int transparentPaletteIndex(struct Palette16 *palette, uint16_t colour, uint16_t transparent);
static void printResults(FILE *file, struct Image *img, struct PaletteOptimisationResults results, const char *prefix, uint16_t transparent, int tilesX, int tilesY, int tileSize);
static struct PaletteOptimiser *optimiserForImage(struct Image *img, int tileSize, int tilesX, int tilesY);

int main(int argc, char **argv)
{
    int statusCode = 0;
    if (argc != 2)
    {
        fprintf(stderr, "Expected exactly 1 argument, usage:\n%s pngfile configFile.h\n", argv[0]);
        return 1;
    }

    bool ok;
    struct Config config = ConfigReader_ReadConfig(argv[1], &ok);
    if (!ok)
    {
        fprintf(stderr, "\nFailed to read config\n");
        return 1;
    }

    struct PaletteOptimiser *optimiser = NULL;

    struct Image *img = Image_New(config.imgFileName);

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

    int tileSize = config.tileSize;

    if (width % tileSize != 0 || height % tileSize != 0)
    {
        fprintf(stderr, "Image width or height not a multiple of the tile size\n");
        statusCode = 1;
        goto exit;
    }

    int tilesX = width / tileSize;
    int tilesY = height / tileSize;

    uint16_t transparent = config.transparentColour;

    optimiser = optimiserForImage(img, tileSize, tilesX, tilesY);
    struct PaletteOptimisationResults results = PaletteOptimiser_OptimisePalettes(optimiser, transparent);

    if (results.nPalettes == 0)
    {
        fprintf(stderr, "Failed to find a set of covering palettes\n");
        statusCode = 1;
        goto exit;
    }

    FILE *output = fopen(config.outFileName, "w");

    if (output == NULL)
    {
        fprintf(stderr, "Failed to open %s for writing\n", config.outFileName);
        statusCode = 1;
        goto exit;
    }

    printResults(output, img, results, config.prefix, transparent, tilesX, tilesY, tileSize);

    fclose(output);

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

static void printResults(FILE *file, struct Image *img, struct PaletteOptimisationResults results, const char *prefix, uint16_t transparent, int tilesX, int tilesY, int tileSize)
{
    fprintf(file, "#include <stdint.h>\n\n");
    fprintf(file, "uint16_t %sPaletteData[256] = {\n", prefix);

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

    fprintf(file, "uint32_t %sTileData[] = {\n", prefix);
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
                    for (int j = innerY * 8; j < innerY * 8 + 8; j++)
                    {
                        fprintf(file, "0x");

                        for (int i = innerX * 8 + 7; i >= innerX * 8; i--)
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
    fprintf(file, "int %sTileDataLength = %d;\n\n", prefix, tileDataLength);

    fprintf(file, "int %sTilePaletteNumber[] = {", prefix);
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