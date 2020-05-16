#include "PaletteOptimiser.h"

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include <stdio.h>

#define MAX_COLOURS 256

static int PaletteOptimiser_addColours(struct PaletteOptimiser *optimiser, struct Palette16 *palette);

struct PaletteOptimiser
{
    struct Palette16 **palettes;
    int nUsedPalettes;
    int nMaxPalettes;

    uint16_t colours[MAX_COLOURS];
    int nColours;
};

struct PaletteOptimiser *PaletteOptimiser_New(int nMaxPalettes)
{
    struct PaletteOptimiser *optimiser = NULL;

    optimiser = calloc(1, sizeof(struct PaletteOptimiser));
    if (!optimiser)
    {
        goto error;
    }

    optimiser->palettes = calloc(nMaxPalettes, sizeof(struct Palette16 *));
    if (!optimiser->palettes)
    {
        goto error;
    }

    optimiser->nMaxPalettes = nMaxPalettes;
    return optimiser;

error:
    if (optimiser)
    {
        free(optimiser->palettes);
    }

    free(optimiser);

    return NULL;
}

void PaletteOptimiser_Free(struct PaletteOptimiser *optimiser)
{
    if (optimiser)
    {
        for (int i = 0; i < optimiser->nUsedPalettes; i++)
        {
            Palette16_Free(optimiser->palettes[i]);
        }

        free(optimiser->palettes);
    }

    free(optimiser);
}

int PaletteOptimiser_AddPalette(struct PaletteOptimiser *optimiser, struct Palette16 *palette)
{
    assert(optimiser->nUsedPalettes < optimiser->nMaxPalettes);

    optimiser->palettes[optimiser->nUsedPalettes++] = palette;
    return PaletteOptimiser_addColours(optimiser, palette);
}

static int PaletteOptimiser_addColour(struct PaletteOptimiser *optimiser, uint16_t colour)
{
    for (int i = 0; i < optimiser->nColours; i++)
    {
        if (optimiser->colours[i] == colour)
        {
            return 0; // we already have this colour
        }
    }

    if (optimiser->nColours == MAX_COLOURS)
    {
        return 1;
    }

    optimiser->colours[optimiser->nColours++] = colour;
    return 0;
}

static int PaletteOptimiser_addColours(struct PaletteOptimiser *optimiser, struct Palette16 *palette)
{
    int nPaletteColours = Palette16_GetNumColours(palette);

    for (int i = 0; i < nPaletteColours; i++)
    {
        uint16_t colour = Palette16_GetColour(palette, i);
        int error = PaletteOptimiser_addColour(optimiser, colour);

        if (error)
        {
            return error;
        }
    }

    return 0;
}

static int PaletteOptimiser_getColourIndex(struct PaletteOptimiser *optimiser, uint16_t colour)
{
    for (int i = 0; i < optimiser->nColours; i++)
    {
        if (optimiser->colours[i] == colour)
        {
            return i;
        }
    }

    assert(0 && "Colour does not exist in colour bank");
}

// Array can contain null values which will be skipped
static struct Palette16 *PaletteOptimiser_findMaximalPaletteFor(struct PaletteOptimiser *optimiser, int nPalettes, struct Palette16 *palettesToSatisfy[nPalettes], uint16_t transparentColour)
{
    struct Palette16 *palette = Palette16_New();
    assert(palette);

    Palette16_AddColour(palette, transparentColour);

    while (true)
    {
        bool aColourIsUsed = false;
        int colourUsage[MAX_COLOURS] = {0};

        // calculate colour usage for each colour in the full colour set
        for (int i = 0; i < nPalettes; i++)
        {
            struct Palette16 *currentPalette = palettesToSatisfy[i];

            if (currentPalette == NULL ||                                                 // this one doesn't need checking any more
                Palette16_UnionLength(palette, currentPalette) > PALETTE16_NUM_COLOURS || // this one cannot be completed with the available colours
                Palette16_Contains(palette, currentPalette) == -1)                        // already satisfied
            {
                continue;
            }

            for (int j = 0; j < Palette16_GetNumColours(currentPalette); j++)
            {
                uint16_t colour = Palette16_GetColour(currentPalette, j);
                if (Palette16_HasColour(palette, colour))
                {
                    continue;
                }

                int colourIndex = PaletteOptimiser_getColourIndex(optimiser, colour);
                colourUsage[colourIndex]++;

                aColourIsUsed = true;
            }
        }

        if (!aColourIsUsed)
        {
            break;
        }

        // select the index with the highest usage
        int bestIndex = 0;
        for (int i = 1; i < optimiser->nColours; i++)
        {
            if (colourUsage[bestIndex] < colourUsage[i])
            {
                bestIndex = i;
            }
        }

        // Add this colour to the working palette
        uint16_t bestColour = optimiser->colours[bestIndex];
        printf("Best colour: %d\n", bestColour);
        if (Palette16_AddColour(palette, bestColour) == PALETTE16_NUM_COLOURS)
        {
            break;
        };
    }

    return palette;
}

// TODO: Select transparent colour
struct PaletteOptimisationResults PaletteOptimiser_OptimisePalettes(struct PaletteOptimiser *optimiser, uint16_t transparentColour)
{
    struct Palette16 **palettes = calloc(MAX_COLOURS / PALETTE16_NUM_COLOURS, sizeof(struct Palette16 *));
    int *assignments = calloc(optimiser->nUsedPalettes, sizeof(int));
    assert(palettes);
    assert(assignments);

    int satisfiedPalettes = 0;
    int currentPaletteNumber = 0;

    struct Palette16 *palettesToSatisfy[optimiser->nUsedPalettes];
    for (int i = 0; i < optimiser->nUsedPalettes; i++)
    {
        palettesToSatisfy[i] = optimiser->palettes[i];
    }

    while (satisfiedPalettes < optimiser->nUsedPalettes)
    {
        struct Palette16 *palette = PaletteOptimiser_findMaximalPaletteFor(optimiser, optimiser->nUsedPalettes, palettesToSatisfy, transparentColour);
        assert(palette);

        for (int i = 0; i < optimiser->nUsedPalettes; i++)
        {
            struct Palette16 *paletteToTest = palettesToSatisfy[i];
            if (paletteToTest == NULL)
            {
                continue;
            }

            if (Palette16_Contains(palette, paletteToTest) != 0)
            {
                assignments[i] = currentPaletteNumber;
                satisfiedPalettes++;
                palettesToSatisfy[i] = NULL;
            }
        }

        palettes[currentPaletteNumber++] = palette;

        if (currentPaletteNumber == MAX_COLOURS / PALETTE16_NUM_COLOURS)
        {
            printf("Failed to find covering palettes");
            break;
        }
    }

    struct PaletteOptimisationResults ret = {
        .nPalettes = currentPaletteNumber,
        .palettes = palettes,
        .paletteAssignment = assignments};

    return ret;
}