#pragma once

#include "Palette.h"

struct PaletteOptimiser;

struct PaletteOptimiser *PaletteOptimiser_New(int nMaxPalettes);
void PaletteOptimiser_Free(struct PaletteOptimiser *optimiser);

// Returns non-zero if there are now too many colours
int PaletteOptimiser_AddPalette(struct PaletteOptimiser *optimiser, struct Palette16 *palette);

struct PaletteOptimisationResults
{
    int nPalettes;
    struct Palette16 **palettes;
    int *paletteAssignment;
};

struct PaletteOptimisationResults PaletteOptimiser_OptimisePalettes(struct PaletteOptimiser *optimiser, uint16_t transparentColour);