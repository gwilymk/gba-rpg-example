#pragma once

#include <stdint.h>

#define PALETTE16_NUM_COLOURS 16

struct Palette16;

struct Palette16 *Palette16_New(void);
void Palette16_Free(struct Palette16 *palette);

int Palette16_GetNumColours(struct Palette16 *palette);
uint16_t Palette16_GetColour(struct Palette16 *palette, int i);

// returns the number of colours in the palette or -1 if it is full
int Palette16_AddColour(struct Palette16 *palette, uint16_t colour);

// Returns -1 if first contains second, 1 if second contains first and 0 if the palettes are disjoint
int Palette16_Contains(struct Palette16 *first, struct Palette16 *second);
