#include <lostgba/ObjectAttribute.h>
#include "LostGbaInternal.h"

struct ObjectAttribute objectAttributeBuffer[ObjectAttributeBuffer_Length];
struct ObjectAffine *objectAffineBuffer = (struct ObjectAffine *)objectAttributeBuffer;

void ObjectAttribute_SetPos(struct ObjectAttribute *attr, int x, int y)
{
    LostGBA_SetBits16(&attr->attr0, y, 8, 0);
    LostGBA_SetBits16(&attr->attr1, x, 9, 0);
}

void ObjectAttribute_SetDisplayMode(struct ObjectAttribute *attr, enum ObjectAttributeDisplayMode displayMode)
{
    LostGBA_SetBits16(&attr->attr0, displayMode, 2, 8);
}

void ObjectAttribute_SetGraphicsMode(struct ObjectAttribute *attr, enum ObjectAttributeGraphicsMode graphicsMode)
{
    LostGBA_SetBits16(&attr->attr0, graphicsMode, 2, 10);
}

void ObjectAttribute_SetMosaicEnabled(struct ObjectAttribute *attr, bool enabled)
{
    LostGBA_SetBits16(&attr->attr0, enabled, 1, 12);
}

void ObjectAttribute_SetColourMode(struct ObjectAttribute *attr, enum ObjectAttributeColourMode colourMode)
{
    LostGBA_SetBits16(&attr->attr0, colourMode, 2, 13);
}

void ObjectAttribute_SetShape(struct ObjectAttribute *attr, enum ObjectAttributeShape shape)
{
    LostGBA_SetBits16(&attr->attr0, shape, 2, 14);
}

void ObjectAttribute_SetHFlip(struct ObjectAttribute *attr, bool hflip)
{
    LostGBA_SetBits16(&attr->attr1, hflip, 1, 12);
}

void ObjectAttribute_SetVFlip(struct ObjectAttribute *attr, bool vflip)
{
    LostGBA_SetBits16(&attr->attr1, vflip, 1, 13);
}

void ObjectAttribute_SetSize(struct ObjectAttribute *attr, enum ObjectAttributeSize size)
{
    LostGBA_SetBits16(&attr->attr1, size, 2, 14);
}

void ObjectAttribute_SetTile(struct ObjectAttribute *attr, u32 tileId)
{
    LostGBA_SetBits16(&attr->attr2, tileId, 10, 0);
}

void ObjectAttribute_SetPriority(struct ObjectAttribute *attr, u16 priority)
{
    LostGBA_SetBits16(&attr->attr2, priority, 2, 10);
}

void ObjectAttribute_SetPaletteBank(struct ObjectAttribute *attr, u16 paletteBank)
{
    LostGBA_SetBits16(&attr->attr2, paletteBank, 4, 12);
}

#define OBJECT_ATTRIBUTE_MEMORY_LOCATION ((struct ObjectAttribute *)(void *)0x07000000)

void ObjectAttributeBuffer_CopyBufferToMemory(void)
{
    struct ObjectAttribute *objectAttributeSystemMemoryLocation = OBJECT_ATTRIBUTE_MEMORY_LOCATION;

    for (int i = 0; i < ObjectAttributeBuffer_Length; i++)
    {
        objectAttributeSystemMemoryLocation[i] = objectAttributeBuffer[i];
    }
}