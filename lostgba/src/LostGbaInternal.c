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
IWRAM_CODE void LostGBA_VMemCpy32_Fast(volatile void *target, const void *src, int words);

ARM_TARGET
IWRAM_CODE void LostGBA_VMemCpy16(volatile void *target, const void *src, int length);

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

#ifdef LOSTGBA_TEST

#include <lostgba/test/Test.h>

LostGBA_Test("MemCpy copies all requested memory if aligned and size divides 32-bits * 32")
{
    u8 src[256];
    u8 target[256] = {0};

    for (int i = 0; i < 256; i++)
    {
        src[i] = i;
    }

    LostGBA_VMemCpy32(target, src, 128);

    for (int i = 0; i < 128; i++)
    {
        LostGBA_Assert(target[i] == i, "Correct bytes were not copied");
    }

    for (int i = 128; i < 256; i++)
    {
        LostGBA_Assert(target[i] == 0, "Not too many bytes were copied");
    }
}

LostGBA_Test("MemCpy copies all requested memory if size is 3 words")
{
    u8 src[256];
    u8 target[256] = {0};

    for (int i = 0; i < 12; i++)
    {
        src[i] = i;
    }

    LostGBA_VMemCpy32(target, src, 12);

    for (int i = 0; i < 12; i++)
    {
        LostGBA_Assert(target[i] == i, "Correct bytes were not copied");
    }

    for (int i = 13; i < 256; i++)
    {
        LostGBA_Assert(target[i] == 0, "Too many bytes were copied");
    }
}

#endif