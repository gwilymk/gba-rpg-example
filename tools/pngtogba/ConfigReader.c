#include "ConfigReader.h"
#include "Palette.h"
#include "Image.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

#define TILESIZE_VAR_NAME "TILESIZE"
#define TRANSPARENT_VAR_NAME "TRANSPARENT"
#define PREFIX_VAR_NAME "PREFIX"

// Returns -1 on an invalid number
static int parseTileSize(const char *tileSizeString)
{
    char *end;
    long int tileSize = strtol(tileSizeString, &end, 10);
    if (tileSize <= 0 || tileSize >= INT32_MAX)
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
    if (colourLong < 0)
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

struct Config ConfigReader_ReadConfig(const char *filename, bool *ok)
{
    *ok = false;

    struct Config config;

    int imageFileNameLen = strrchr(filename, '.') - filename;
    config.imgFileName = malloc(imageFileNameLen + 1);
    strncpy(config.imgFileName, filename, imageFileNameLen);
    config.imgFileName[imageFileNameLen] = '\0';

    config.outFileName = malloc(imageFileNameLen + 3); // .c\0
    strncpy(config.outFileName, filename, imageFileNameLen);
    strncpy(config.outFileName + imageFileNameLen, ".c", 3);

    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        fprintf(stderr, "Failed to open file %s", filename);
        return config;
    }

#define BUFFER_SIZE 256
    char buffer[256];
    int bytesRead = fread(buffer, 1, BUFFER_SIZE - 1, file);
    fclose(file);

    if (bytesRead <= 0)
    {
        fprintf(stderr, "Failed to read anything from file %s", filename);
        return config;
    }

    buffer[bytesRead] = '\0';

    // -------- Extract tile size ------------
    char *tileSizeVar = strstr(buffer, TILESIZE_VAR_NAME "=");
    if (tileSizeVar == NULL)
    {
        fprintf(stderr, "Tile size required. Please include " TILESIZE_VAR_NAME "=<tilesize> in the first 256 characters of your file");
        return config;
    }

    char *tileSizeLocation = tileSizeVar + strlen(TILESIZE_VAR_NAME "=");
    config.tileSize = parseTileSize(tileSizeLocation);

    if (config.tileSize == -1)
    {
        fprintf(stderr, "Tile size provided is invalid");
        return config;
    }

    // -------- Extract prefix ------------
    char *prefixVar = strstr(buffer, PREFIX_VAR_NAME "=");
    if (prefixVar == NULL)
    {
        fprintf(stderr, "Prefix required. Please include " PREFIX_VAR_NAME "=<prefix in the first 256 characters of your file");
        return config;
    }

    char *prefixVarLocation = prefixVar + strlen(PREFIX_VAR_NAME "=");
    int prefixValueLen = 0;
    while (isalnum(prefixVarLocation[prefixValueLen]))
    {
        prefixValueLen++;
    }

    config.prefix = malloc(prefixValueLen + 1);
    strncpy(config.prefix, prefixVarLocation, prefixValueLen);
    config.prefix[prefixValueLen] = '\0';

    // -------- Extract transparent colour ------------
    config.transparentColour = INVALID_COLOUR;
    char *transparentVar = strstr(buffer, TRANSPARENT_VAR_NAME "=");
    if (transparentVar == NULL)
    {
        *ok = true;
        return config;
    }

    char *transparentColourLocation = transparentVar + strlen(TRANSPARENT_VAR_NAME "=");
    config.transparentColour = parseColour(transparentColourLocation);

    if (config.transparentColour == INVALID_COLOUR)
    {
        fprintf(stderr, "Failed to parse transparent colour");
        return config;
    }

    *ok = true;
    return config;
}