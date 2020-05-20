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

ARM_TARGET
void LostGBA_VMemCpy32_Fast(volatile void *target, const void *src, int words);

ARM_TARGET
void LostGBA_VMemCpy16(volatile void *target, const void *src, int length);

void LostGBA_VMemCpy32(volatile void *target, const void *src, int length)
{
    if (length == 0)
    {
        return; // nothing to do
    }

    if (length % 4 == 0)
    {
        // whole number of words long. So we can do lots at a time
        LostGBA_VMemCpy32_Fast(target, src, length / 4);
        return;
    }

    LostGBA_VMemCpy16(target, src, length);
}