/**
 * @file Graphics.h
 * @brief Basic routines for setting up the GBA's graphics
 * 
 * @defgroup GRAPHICS GBA graphics management
 * @{
 */

#pragma once

#include "GbaTypes.h"

/** 
 * @brief The possible graphics modes for the GBA. 
 * 
 * In different modes, different backgrounds are available and the types of the backgrounds differ too.
 * 
 * mode | BG0 | BG1 | BG2 | BG3
 * -----|-----|-----|-----|----
 * 0    | reg | reg | reg | reg
 * 1    | reg | reg | aff | -
 * 2    | -   | -   | aff | aff
 */
enum GraphicsMode
{
    GraphicsMode_0, /**< All 4 backgrounds are tile mode */
    GraphicsMode_1, /**< Backgrounds 0 and 1 are tile, background 2 is affine */
    GraphicsMode_2, /**< Backgrounds 2 and 3 are affine and backgrounds 0 and 1 are unavailable */
    GraphicsMode_3, /**< 16-bit bitmapped 240x160 mode */
    GraphicsMode_4, /**< 8-bit palette 240x160 mode */
    GraphicsMode_5  /**< 16-bit bitmapped 160x128 mode */
};

/**
 * @brief Full graphics settings control
 * 
 * This can be changed at any time by calling Graphics_SetMode()
 */
struct GraphicsSettings
{
    enum GraphicsMode graphicsMode;
    bool enableBG0;
    bool enableBG1;
    bool enableBG2;
    bool enableBG3;
    bool enableSprites;
} LOSTGBA_PACKED_ALIGN(4);

/** Sets the graphics mode */
void Graphics_SetMode(struct GraphicsSettings graphicsSettings);

/** Controls whether we should trigger vblank interrupts */
void Graphics_SetVBlankInterrupt(bool enabled);

#define Graphics_ScreenWidth 240
#define Graphics_ScreenHeight 160

/** @} */