#pragma once

struct Image;

struct Image *Image_New(const char *filename);
char *Image_Error(struct Image *img);
void Image_Free(struct Image *img);

int Image_Width(struct Image *img);
int Image_Height(struct Image *img);