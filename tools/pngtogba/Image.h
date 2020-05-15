#pragma once

struct Image;

struct Colour
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct Image *Image_New(const char *filename);
char *Image_Error(struct Image *img);
void Image_Free(struct Image *img);

int Image_Width(struct Image *img);
int Image_Height(struct Image *img);

struct Colour Image_Colour(struct Image *img, int x, int y);