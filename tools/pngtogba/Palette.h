#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PALETTE16_NUM_COLOURS 16
#define INVALID_COLOUR (1 << 15)

struct Palette16;

struct Palette16 *Palette16_New(void);
void Palette16_Free(struct Palette16 *palette);

int Palette16_GetNumColours(struct Palette16 *palette);
uint16_t Palette16_GetColour(struct Palette16 *palette, int i);

// returns the number of colours in the palette or -1 if it is full
int Palette16_AddColour(struct Palette16 *palette, uint16_t colour);

// Returns -1 if first contains second, 1 if second contains first and 0 if the palettes are disjoint. Will return -1 if equal
int Palette16_Contains(struct Palette16 *first, struct Palette16 *second);

bool Palette16_HasColour(struct Palette16 *palette, uint16_t colour);

int Palette16_UnionLength(struct Palette16 *first, struct Palette16 *second);