#pragma once

#include <stdint.h>
#include <stdbool.h>

struct Config
{
    int tileSize;
    uint16_t transparentColour;

    char *imgFileName;
    char *outFileName;
    char *prefix;
};

struct Config ConfigReader_ReadConfig(const char *filename, bool *ok);