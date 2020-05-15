#include "Image.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Expected exactly 1 argument, usage:\n%s pngfile", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    struct Image *img = Image_New(filename);

    char *error = NULL;
    if (img == NULL || (error = Image_Error(img)) != NULL)
    {
        fprintf(stderr, "Failed to load image\n");
        if (error)
        {
            fprintf(stderr, "Error: %s", error);
        }

        return 1;
    }

    Image_Free(img);
}