#include "LostGbaInternal.h"

void LostGBA_SetBits16(u16 *target, u16 value, u16 length, u16 shift)
{
    u16 mask = LostGBA_AllOnes16(length);
    (*target) = (*target & ~(mask << shift)) | ((value & mask) << shift);
}