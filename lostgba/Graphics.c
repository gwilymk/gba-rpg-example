#include <lostgba/Graphics.h>
#include "LostGbaInternal.h"

static vu16 *Graphics_displayControlRegister = (vu16 *)0x04000000;

void Graphics_SetMode(struct GraphicsSettings settings)
{
    bool sprites1d = true;

    u16 mode = (settings.graphicsMode & LostGBA_AllOnes16(3)) |
               (sprites1d << 6) |
               (settings.enableBG0 << 8) |
               (settings.enableBG1 << 9) |
               (settings.enableBG2 << 10) |
               (settings.enableBG3 << 11) |
               (settings.enableSprites << 12);

    *Graphics_displayControlRegister = mode;
}

static vu16 *Graphics_displayStatusRegister = (vu16 *)0x04000004;

void Graphics_SetVBlankInterrupt(bool enabled)
{
    *Graphics_displayStatusRegister |= enabled << 3;
}

static u16 *Graphics_blendingModeRegister = (u16 *)0x04000050;

void Graphics_SetBlendingMode(enum GraphicsBlendingMode blendingMode)
{
    LostGBA_SetBits16(Graphics_blendingModeRegister, blendingMode, 2, 6);
}