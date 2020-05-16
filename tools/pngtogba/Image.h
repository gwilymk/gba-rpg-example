#pragma once

#include <stdint.h>

struct Image;

struct Colour
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct Image *Image_New(const char *filename);
char *Image_Error(struct Image *img);
void Image_Free(struct Image *img);

int Image_Width(struct Image *img);
int Image_Height(struct Image *img);

struct Colour Image_Colour(struct Image *img, int x, int y);

inline uint16_t rgb15(struct Colour c)
{
    return ((c.r >> 3) & 31) | (((c.g >> 3) & 31) << 5) | (((c.b >> 3) & 31) << 10);
}