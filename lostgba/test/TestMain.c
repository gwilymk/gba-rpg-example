#include <lostgba/test/Test.h>

#include <lostgba/Graphics.h>
#include <lostgba/TileMap.h>
#include <lostgba/Background.h>

#include <string.h>

struct RegisteredTest
{
    LostGBA_TestMethod testMethod;
    const char *testName;
};

#define MAX_TESTS 256

static struct RegisteredTest RegisteredTests[MAX_TESTS];
static int NumRegisteredTests;

void LostGBA_TestRegister(LostGBA_TestMethod testMethod, const char *testName)
{
    RegisteredTests[NumRegisteredTests].testMethod = testMethod;
    RegisteredTests[NumRegisteredTests].testName = testName;

    NumRegisteredTests++;
}

struct AgbPrintContext
{
    u16 request;
    u16 bank;
    u16 get;
    u16 put;
};

static volatile u16 *AgbPrintBuffer = (volatile u16 *)0x09fd0000;
static volatile struct AgbPrintContext *AgbPrintContext = (volatile struct AgbPrintContext *)0x09fe20f8;
static volatile u16 *AgbPrintProtect = (volatile u16 *)0x09fe2ffe;

static void agbPrintInit(void)
{
    *AgbPrintProtect = 0x20;
    AgbPrintContext->request = 0;
    AgbPrintContext->get = 0;
    AgbPrintContext->put = 0;
    AgbPrintContext->bank = 0xfd;
    *AgbPrintProtect = 0;
}

static void agbPutChar(char c)
{
    u16 data = AgbPrintBuffer[AgbPrintContext->put / 2];

    *AgbPrintProtect = 0x20;
    data = (AgbPrintContext->put % 2) ? (c << 8) | (data & 0xff) : (data & 0xff00) | c;
    AgbPrintBuffer[AgbPrintContext->put / 2] = data;
    AgbPrintContext->put++;
    *AgbPrintProtect = 0;
}

static void agbPrint(const char *text)
{
    agbPrintInit();
    while (*text)
    {
        agbPutChar(*text);
        text++;
    }

    asm volatile("swi 0xfa");
}

void LostGBA_TestAssertFailed(const char *testName, const char *fileName, int lineNumber, const char *message)
{
    agbPrint(testName);
    agbPrint(" failed with message ");
    agbPrint(message);

    while (1)
    {
    }
}

static void setupBackground(void)
{
#define RED 0x001F
#define GREEN 0x03E0
    u16 paletteData[256] = {
        RED, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        GREEN, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0};
    TileMap_CopyToBackgroundPalette(paletteData);

    u32 tileData[32] = {0};
    TileMap_CopyToBackgroundTiles(0, &tileData[0], sizeof(tileData));

    Background_SetColourMode(BackgroundNumber_0, BackgroundColourMode_4PP);
    Background_SetScreenBaseBlock(BackgroundNumber_0, 20);

    Background_SetSize(BackgroundNumber_0, BackgroundSize_32x32);
    Background_SetTileBackgroundNumber(BackgroundNumber_0, 0);

    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            Background_SetTile(20, BackgroundSize_32x32, x, y, 0, false, false, 1);
        }
    }
}

int main(void)
{
    struct GraphicsSettings settings = {
        .graphicsMode = GraphicsMode_0,
        .enableBG0 = true,
    };
    Graphics_SetMode(settings);

    setupBackground();

    for (int i = 0; i < NumRegisteredTests; i++)
    {
        RegisteredTests[i].testMethod(RegisteredTests[i].testName);
    }

    agbPrint("All tests passed");
    while (1)
    {
    }
}