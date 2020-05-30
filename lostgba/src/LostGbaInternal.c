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

void LostGBA_VMemCpy(volatile void *target, const void *src, int length);

void LostGBA_VMemCpy32(volatile void *target, const void *src, int length)
{
    LostGBA_VMemCpy(target, src, length);
}

#ifdef LOSTGBA_TEST

#include <lostgba/test/Test.h>

#define MemoryTest(startSrc, startTarget, toCopy)                                                       \
    do                                                                                                  \
    {                                                                                                   \
        int startTarget_ = (startTarget);                                                               \
        int startSrc_ = (startSrc);                                                                     \
        int toCopy_ = (toCopy);                                                                         \
        u8 src[256];                                                                                    \
        u8 target[256] = {0};                                                                           \
        for (int i = 0; i < 256; i++)                                                                   \
        {                                                                                               \
            src[i] = i;                                                                                 \
        }                                                                                               \
        LostGBA_VMemCpy32(target + startTarget_, src + startSrc_, toCopy_);                             \
        for (int i = startTarget_; i < startTarget_ + toCopy_; i++)                                     \
        {                                                                                               \
            LostGBA_Assert(target[i] == startSrc_ + i - startTarget_, "Correct bytes were not copied"); \
        }                                                                                               \
        for (int i = 0; i < startTarget_; i++)                                                          \
        {                                                                                               \
            LostGBA_Assert(target[i] == 0, "Bytes were copied before intended start point");            \
        }                                                                                               \
        for (int i = startTarget_ + toCopy; i < 256; i++)                                               \
        {                                                                                               \
            LostGBA_Assert(target[i] == 0, "Too many bytes were copied");                               \
        }                                                                                               \
    } while (0)

LostGBA_Test("MemCpy copies all requested memory if aligned and size divides 32-bits * 32")
{
    MemoryTest(0, 0, 128);
}

LostGBA_Test("MemCpy copies all requested memory if size is 3 words")
{
    MemoryTest(0, 0, 12);
}

LostGBA_Test("MemCpy copies all requested memory if size is 3.5 words")
{
    MemoryTest(0, 0, 14);
}

LostGBA_Test("MemCpy copies all requested memory if size is 0.5 words")
{
    MemoryTest(0, 0, 2);
}

LostGBA_Test("MemCpy copies all requested memory if both source and target are mis-aligned")
{
    MemoryTest(2, 2, 126);
}

LostGBA_Test("MemCpy copies all requested memory if both source and target are mis-aligned including last half word")
{
    MemoryTest(2, 2, 128);
}

LostGBA_Test("MemCpy copies all requested memory if both source but not target are mis-aligned")
{
    MemoryTest(2, 0, 128);
}

#endif