#include <lostgba/TileMap.h>
#include "LostGbaInternal.h"

#include <string.h>

#define SPRITE_PALETTE_MEMORY_LOCATION ((vu16 *)0x05000200)

void TileMap_CopyToSpritePalette(const u16 paletteData[TileMap_PaletteLength])
{
    LostGBA_VMemCpy(SPRITE_PALETTE_MEMORY_LOCATION, paletteData, TileMap_PaletteLength * sizeof(u16));
}

#define SPRITE_CHARBLOCK_BASE ((vu16 *)0x06010000)
#define CHARBLOCK_SIZE 0x4000

void LOSTGBA_UNSAFE(TileMap_CopyToSpriteTiles)(int tileNumber, const u32 *tileData, int length)
{
    LostGBA_VMemCpy(SPRITE_CHARBLOCK_BASE + tileNumber * CHARBLOCK_SIZE, tileData, length);
}

#define BG_PALETTE_MEMORY_LOCATION ((vu16 *)0x05000000)

void TileMap_CopyToBackgroundPalette(const u16 paletteData[TileMap_PaletteLength])
{
    LostGBA_VMemCpy(BG_PALETTE_MEMORY_LOCATION, paletteData, TileMap_PaletteLength * sizeof(u16));
}

#define TILE_MEMORY_LOCATION ((vu16 *)0x06000000)

void LOSTGBA_UNSAFE(TileMap_CopyToBackgroundTiles)(int tileNumber, const u32 *tileData, int length)
{
    LostGBA_VMemCpy(TILE_MEMORY_LOCATION + tileNumber * CHARBLOCK_SIZE, tileData, length);
}
