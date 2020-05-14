#include <lostgba/Graphics.h>
#include <lostgba/Interrupt.h>
#include <lostgba/TileMap.h>
#include <lostgba/SystemCalls.h>
#include <lostgba/Background.h>
#include <lostgba/Input.h>
#include <lostgba/ObjectAttribute.h>

#include "images/tileset.h"
#include "images/character.h"
#include "tilemaps/world.h"

int main(void)
{
    Interrupt_Init();
    Interrupt_EnableType(InterruptType_VBlank);
    Interrupt_Enable();

    struct GraphicsSettings settings = {
        .graphicsMode = GraphicsMode_0,
        .enableBG0 = true,
        .enableBG1 = true,
        .enableSprites = true};

    Graphics_SetMode(settings);

    TileMap_CopyToBackgroundTiles(0, tilesetTiles, tilesetTilesLen);
    TileMap_CopyToBackgroundPalette(tilesetPal);
    TileMap_CopyToSpritePalette(characterPal);
    TileMap_CopyToSpriteTiles(0, characterTiles, characterTilesLen);

    Background_SetColourMode(BackgroundNumber_0, BackgroundColourMode_8PP);
    Background_SetSize(BackgroundNumber_0, BackgroundSize_64x64);
    Background_SetScreenBaseBlock(BackgroundNumber_0, 20);
    Background_SetTileBackgroundNumber(BackgroundNumber_0, 0);
    Background_SetPriority(BackgroundNumber_0, 1);

    Background_SetColourMode(BackgroundNumber_1, BackgroundColourMode_8PP);
    Background_SetSize(BackgroundNumber_1, BackgroundSize_64x64);
    Background_SetScreenBaseBlock(BackgroundNumber_1, 24);
    Background_SetTileBackgroundNumber(BackgroundNumber_1, 0);
    Background_SetPriority(BackgroundNumber_1, 0);

    *((u16 *)0x04000050) = 0b00010001100000111;

    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            int tile = worldTilemap[x + y * 64];
            Background_SetTile(20, BackgroundSize_64x64, x, y, tile, false, false, 0);

            switch (tile)
            {
            case 5:
            case 6:
            case 8:
            case 9:
            case 24:
            case 25:
            case 40:
            case 41:
                Background_SetTile(24, BackgroundSize_64x64, x, y, tile, false, false, 0);
                break;
            default:
                Background_SetTile(24, BackgroundSize_64x64, x, y, 0, false, false, 0);
                break;
            }
        }
    }

    for (int i = 0; i < ObjectAttributeBuffer_Length; i++)
    {
        ObjectAttribute_SetDisplayMode(&objectAttributeBuffer[i], ObjectAttributeDisplayMode_Hidden);
    }

    struct ObjectAttribute *character = &objectAttributeBuffer[0];

    ObjectAttribute_SetGraphicsMode(character, ObjectAttributeGraphicsMode_Normal);
    ObjectAttribute_SetDisplayMode(character, ObjectAttributeDisplayMode_Normal);
    ObjectAttribute_SetPaletteBank(character, 0);
    ObjectAttribute_SetColourMode(character, ObjectAttributeColourMode_4PP);
    ObjectAttribute_SetSize(character, ObjectAttributeSize_16);
    ObjectAttribute_SetShape(character, ObjectAttributeShape_Square);
    ObjectAttribute_SetPriority(character, 1);

    ObjectAttribute_SetPos(character, Graphics_ScreenWidth / 2, Graphics_ScreenHeight / 2);

    int x = 0;
    int y = 0;

    int frame = 0;
    int frameSkip = 0;
#define LOOPS_PER_FRAME 3

#define SPEED 1

#define DOWN_START 3
#define DOWN_END 10
#define RIGHT_START 11
#define RIGHT_END 21
#define LEFT_START 22
#define LEFT_END 32
#define UP_START 34
#define UP_END 41

#define DOWN_IDLE 1
#define RIGHT_IDLE 11
#define LEFT_IDLE 22
#define UP_IDLE 33

    int currentFrame = DOWN_IDLE;
    enum Direction
    {
        Direction_Up,
        Direction_Left,
        Direction_Down,
        Direction_Right
    } direction = Direction_Down;

    while (true)
    {
        SystemCall_WaitForVBlank();

        Input_UpdateKeyState();

        bool moving = false;
        if (Input_IsKeyDown(InputKey_Up))
        {
            y -= SPEED;
            if (frame >= UP_END - UP_START)
            {
                frame = 0;
            }
            currentFrame = UP_START + frame - 1;

            moving = true;
            direction = Direction_Up;
        }

        if (Input_IsKeyDown(InputKey_Down))
        {
            y += SPEED;
            if (frame >= DOWN_END - DOWN_START)
            {
                frame = 0;
            }
            currentFrame = DOWN_START + frame - 1;

            moving = true;
            direction = Direction_Down;
        }

        if (Input_IsKeyDown(InputKey_Left))
        {
            x -= SPEED;
            if (frame >= LEFT_END - LEFT_START)
            {
                frame = 0;
            }
            currentFrame = LEFT_START + frame - 1;

            moving = true;
            direction = Direction_Left;
        }

        if (Input_IsKeyDown(InputKey_Right))
        {
            x += SPEED;
            if (frame >= RIGHT_END - RIGHT_START)
            {
                frame = 0;
            }
            currentFrame = RIGHT_START + frame - 1;

            moving = true;
            direction = Direction_Right;
        }

        if (!moving)
        {
            switch (direction)
            {
            case Direction_Up:
                currentFrame = UP_IDLE - 1;
                break;
            case Direction_Down:
                currentFrame = DOWN_IDLE - 1;
                break;
            case Direction_Left:
                currentFrame = LEFT_IDLE - 1;
                break;
            case Direction_Right:
                currentFrame = RIGHT_IDLE - 1;
                break;
            }
        }

        frameSkip++;

        if (frameSkip == LOOPS_PER_FRAME)
        {
            frame++;
            frameSkip = 0;
        }

        ObjectAttribute_SetTile(character, currentFrame * 4);

        Background_SetHorizontalOffset(BackgroundNumber_0, x);
        Background_SetVerticalOffset(BackgroundNumber_0, y);
        Background_SetHorizontalOffset(BackgroundNumber_1, x);
        Background_SetVerticalOffset(BackgroundNumber_1, y);

        ObjectAttributeBuffer_CopyBufferToMemory();
    }
}