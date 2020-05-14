#include <lostgba/Graphics.h>
#include <lostgba/Interrupt.h>
#include <lostgba/TileMap.h>
#include <lostgba/SystemCalls.h>
#include <lostgba/Background.h>

#include "images/tileset.h"
#include "tilemaps/world.h"

int main(void)
{
    Interrupt_Init();
    Interrupt_EnableType(InterruptType_VBlank);
    Interrupt_Enable();

    struct GraphicsSettings settings = {
        .graphicsMode = GraphicsMode_0,
        .enableBG0 = true};

    Graphics_SetMode(settings);

    TileMap_CopyToBackgroundTiles(0, tilesetTiles, tilesetTilesLen);
    TileMap_CopyToBackgroundPalette(tilesetPal);

    Background_SetColourMode(BackgroundNumber_0, BackgroundColourMode_8PP);
    Background_SetSize(BackgroundNumber_0, BackgroundSize_64x64);
    Background_SetScreenBaseBlock(BackgroundNumber_0, 20);
    Background_SetTileBackgroundNumber(BackgroundNumber_0, 0);

    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            Background_SetTile(20, BackgroundSize_64x64, x, y, worldTilemap[x + y * 64], false, false, 0);
        }
    }

    while (true)
    {
        SystemCall_WaitForVBlank();
    }
}