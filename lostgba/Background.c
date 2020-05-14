#include <lostgba/Background.h>
#include "LostGbaInternal.h"

u16 *Background_ControlRegisterBaseAddr = (u16 *)0x04000008;

static void Background_setBits(enum BackgroundNumber backgroundNumber, u16 value, u16 length, u16 shift)
{
    LostGBA_SetBits16(&Background_ControlRegisterBaseAddr[backgroundNumber], value, length, shift);
}

void Background_SetPriority(enum BackgroundNumber backgroundNumber, int priority)
{
    Background_setBits(backgroundNumber, priority, 2, 0);
}

void Background_SetTileBackgroundNumber(enum BackgroundNumber backgroundNumber, int tileBackgroundNumber)
{
    Background_setBits(backgroundNumber, tileBackgroundNumber, 2, 2);
}

void Background_SetColourMode(enum BackgroundNumber backgroundNumber, enum BackgroundColourMode colourMode)
{
    Background_setBits(backgroundNumber, colourMode, 1, 7);
}

void LOSTGBA_UNSAFE(Background_SetScreenBaseBlock)(enum BackgroundNumber backgroundNumber, int screenblock)
{
    Background_setBits(backgroundNumber, screenblock, 5, 8);
}

void Background_SetSize(enum BackgroundNumber backgroundNumber, enum BackgroundSize backgroundSize)
{
    Background_setBits(backgroundNumber, backgroundSize, 2, 14);
}

static u16 Background_makeScreenEntry(int tileId, bool hflip, bool vflip, int paletteBank)
{
    return (tileId & LostGBA_AllOnes16(10)) |
           (hflip << 10) |
           (vflip << 11) |
           (paletteBank << 12);
}

static int Background_screenBlockOffset(enum BackgroundSize backgroundSize, int x, int y)
{
    switch (backgroundSize)
    {
    case BackgroundSize_32x32:
        return 0;
    case BackgroundSize_32x64:
        return y >= 32;
    case BackgroundSize_64x32:
        return x >= 32;
    case BackgroundSize_64x64:
        return x / 32 + 2 * (y / 32);
    default:
        LOSTGBA_UNREACHABLE();
    }
}

#define VRAM_BASE ((u16 *)0x06000000)
#define SCREEN_BLOCK_LENGTH 1024

void LOSTGBA_UNSAFE(Background_SetTile)(int screenBaseBlock, enum BackgroundSize backgroundSize, int x, int y, int tileId, bool hflip, bool vflip, int paletteBank)
{
    u16 screenEntry = Background_makeScreenEntry(tileId, hflip, vflip, paletteBank);

    int screenBlockStep = (x % 32) + (y % 32) * 32;
    int screenBlockOffset = Background_screenBlockOffset(backgroundSize, x, y);

    *(VRAM_BASE + (SCREEN_BLOCK_LENGTH * (screenBaseBlock + screenBlockOffset)) + screenBlockStep) = screenEntry;
}