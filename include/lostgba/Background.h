/**
 * @file background.h
 * @brief Handles everything you'd ever want to handle with backgrounds (except the tilemap)
 * 
 * @defgroup BACKGROUNDS Backgrounds
 * @{
 * 
 * The valid backgrounds are numbered 0 - 3 but which ones are available and which 'types' they
 * are depends on the current graphics mode.
 * 
 * mode | BG0 | BG1 | BG2 | BG3
 * -----|-----|-----|-----|----
 * 0    | reg | reg | reg | reg
 * 1    | reg | reg | aff | -
 * 2    | -   | -   | aff | aff
 * 
 * They must also be explicitly enabled as part of the current graphics mode.
 */

#pragma once

#include "GbaTypes.h"
#include "LostGbaUtil.h"

/** Background number enum. Used to force people to think harder about which background number they are using */
enum BackgroundNumber
{
    BackgroundNumber_0, /**< Background number 0 */
    BackgroundNumber_1, /**< Background number 1 */
    BackgroundNumber_2, /**< Background number 2 */
    BackgroundNumber_3  /**< Background number 3 */
};

/** Set the priority of a given background (0 - 4) */
void Background_SetPriority(enum BackgroundNumber backgroundNumber, int priority);
/** Set the background number (character base block). TileMap_CopyToBackgroundTiles sets the tiles themselves */
void Background_SetTileBackgroundNumber(enum BackgroundNumber backgroundNumber, int tileBackgroundNumber);

/** The colour mode of the background */
enum BackgroundColourMode
{
    /** 4pp (16 colours). Can choose the sub-palette for each tile */
    BackgroundColourMode_4PP,
    /** 8pp (256 colours) */
    BackgroundColourMode_8PP
};

/** Set the current colour mode of the background */
void Background_SetColourMode(enum BackgroundNumber backgroundNumber, enum BackgroundColourMode colourMode);
/** Set the current screenblock for the given background. This is what determines which tiles get placed where */
#define Background_SetScreenBaseBlock(backgroundNumber, screenBlock)                                             \
    do                                                                                                           \
    {                                                                                                            \
        _Static_assert(0 <= screenBlock && screenBlock <= 31, "Screenblock must be between 0 and 31 inclusive"); \
        LOSTGBA_UNSAFE(Background_SetScreenBaseBlock)                                                            \
        (backgroundNumber, screenBlock);                                                                         \
    } while (0)
/** Unsafe version of Background_SetScreenBlock */
void LOSTGBA_UNSAFE(Background_SetScreenBaseBlock)(enum BackgroundNumber backgroundNumber, int screenblock);

/** Background size flags. The map layout is a bit strange if you pick higher than 32x32 tiles. */
enum BackgroundSize
{
    /**
     * @brief 32 x 32 tiles (or 256 x 256 pixels)
     * 
     * The actual tiles will be defined by the 32x32 entries in the base block for this background
     */
    BackgroundSize_32x32,
    /**
     * @brief 64 x 32 tiles (or 512 x 256 pixels)
     * 
     * The actual tiles used are in 2 blocks of 32 x 32 tiles arranged as follows:
     * <table>
     * <tr><td>BaseBlock</td><td>BaseBlock + 1</td></tr>
     * </table>
     */
    BackgroundSize_64x32,
    /**
     * @brief 32 x 64 tiles (or 256 x 512 pixels)
     * 
     * The actual tiles used are in 2 blocks of 32 x 32 arranged as follows:
     * <table>
     * <tr><td>BaseBlock</td></tr>
     * <tr><td>BaseBlock + 1</td></tr>
     * </table>
     */
    BackgroundSize_32x64,
    /**
     * @brief 64 x 64 tiles (or 512 x 512 pixels)
     * 
     * The actual tiles used are in 4 blocks of 32 x 32 arranged as follows:
     * <table>
     * <tr><td>BaseBlock</td><td>BaseBlock + 1</td></tr>
     * <tr><td>BaskBlock + 2</td><td>BaseBlock + 3</td></tr>
     * </table>
     */
    BackgroundSize_64x64
};
/** Set the background size for the given background */
void Background_SetSize(enum BackgroundNumber backgroundNumber, enum BackgroundSize backgroundSize);

/**
 * @brief Set the tile to the given tileId
 * 
 * @param baseBlock The base block that the background has been set to
 * @param backgroundSize The size of the background (needed to turn x, y into coordinate ids)
 * @param x The x location in the tilemap
 * @param y The y location in the tilemap
 * @param hflip Whether the tile should be flipped horizontally
 * @param vflip Whether the tile should be filpped vertically
 * @param paletteBank Which palette bank to use
 * 
 * This method takes into account the size of the background, so you can refer to the locations in further
 * screen blocks without needing to fiddle with which block it needs to go into.
 */
#define Background_SetTile(baseBlock, backgroundSize, x, y, tileId, hflip, vflip, paletteBank)              \
    do                                                                                                      \
    {                                                                                                       \
        _Static_assert(0 <= baseBlock && baseBlock <= 31, "Base block must be between 0 and 31 inclusive"); \
        LOSTGBA_UNSAFE(Background_SetTile)                                                                  \
        (baseBlock, backgroundSize, x, y, tileId, hflip, vflip, paletteBank);                               \
    } while (0)
/** Unsafe version of Background_SetTile */
void LOSTGBA_UNSAFE(Background_SetTile)(int screenBaseBlock, enum BackgroundSize backgroundSize, int x, int y, int tileId, bool hflip, bool vflip, int paletteBank);
/** @} */