/**
 * @file TileMap.h
 * @brief Handle tilemaps and palettes for sprites and backgrounds
 * 
 * @defgroup TILEMAP Tile maps
 * @{
 */

#pragma once

#include "GbaTypes.h"
#include "LostGbaUtil.h"

/** The size of a single palette */
#define TileMap_PaletteLength 256

/**
 * @brief Copies the provided palette data to the sprite pallete memory location.
 * 
 * This is stored at memory location `0x05000200`.
 */
void TileMap_CopyToSpritePalette(const u16 paletteData[TileMap_PaletteLength]);
/**
 * @brief Safe version of copy sprite tiles
 * @param tileNumber The sprite background block number. Either 0 or 1
 * @param tileData The tiledata to store
 * @param length The length of the tile data in bytes
 * 
 * This calls LOSTGBA_UNSAFE(TileMap_CopyToSpriteTiles) but does a static assert to check that you're not copying
 * out of range. Use the unsafe version if you are using dynamic values (but ideally check that they are valid)
 * somewhere else
 */
#define TileMap_CopyToSpriteTiles(tileNumber, tileData, length)                                          \
    do                                                                                                   \
    {                                                                                                    \
        _Static_assert(0 <= tileNumber && tileNumber <= 1, "Sprites can only use tile numbers 0 and 1"); \
        LOSTGBA_UNSAFE(TileMap_CopyToSpriteTiles)                                                        \
        (tileNumber, tileData, length);                                                                  \
    } while (0)
/**
 * The unsafe version of TileMap_CopyToSpriteTiles
 */
void LOSTGBA_UNSAFE(TileMap_CopyToSpriteTiles)(int tileNumber, const unsigned int *tileData, int length);

/**
 * @brief Copies the provided palette data to the background palette memory location
 * 
 * This is stored at memory location `0x05000000`
 */
void TileMap_CopyToBackgroundPalette(const u16 paletteData[TileMap_PaletteLength]);
/**
 * @brief The safe version of copy to background tiles
 * @param backgroundNumber The tile block number to use. Must be between 0 and 3 inclusive.
 * @param tileData The tile data to copy to this background tile.
 * @param length The length of tileData in bytes
 * 
 * This calls LOSTGBA_UNSAFE(TileMap_CopyToBackgroundTiles) after static asserts on the arguments to make
 * sure that they are sane.
 */
#define TileMap_CopyToBackgroundTiles(backgroundNumber, tileData, length)                                                      \
    do                                                                                                                         \
    {                                                                                                                          \
        _Static_assert(0 <= backgroundNumber && backgroundNumber <= 3, "Background number must be between 0 and 3 inclusive"); \
        LOSTGBA_UNSAFE(TileMap_CopyToBackgroundTiles)                                                                          \
        (backgroundNumber, tileData, length);                                                                                  \
    } while (0)
/**
 * The unsafe version of TileMap_CopyToBackgroundPalette
 */
void LOSTGBA_UNSAFE(TileMap_CopyToBackgroundTiles)(int backgroundNumber, const unsigned int *tileData, int length);

/** @} */