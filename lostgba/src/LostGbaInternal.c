#include "LostGbaInternal.h"

void LostGBA_SetBits16(u16 *target, u16 value, u16 length, u16 shift)
{
    u16 mask = LostGBA_AllOnes16(length);
    (*target) = (*target & ~(mask << shift)) | ((value & mask) << shift);
}

void LostGBA_SetVBits16(vu16 *target, u16 value, u16 length, u16 shift)
{
    u16 mask = LostGBA_AllOnes16(length);
    (*target) = (*target & ~(mask << shift)) | ((value & mask) << shift);
}

void LostGBA_VMemCpy32(volatile void *target, const void *src, int length)
{
    // do I need to worry about memory aliasing here?
    s32 *srcInts = (s32 *)src;
    volatile s32 *targetInts = (volatile s32 *)target;
    for (int i = 0; i < length / 4; i++)
    {
        targetInts[i] = srcInts[i];
    }

    s8 *srcChars = (s8 *)src;
    volatile s8 *targetChars = (volatile s8 *)target;
    for (int i = length % 4 - 1; i >= 0; i--)
    {
        targetChars[length - i] = srcChars[length - i];
    }
}