/**
 * @file LostGbaInternal.h
 */

#pragma once

#include <lostgba/GbaTypes.h>

#define LOSTGBA_UNREACHABLE()    \
    do                           \
    {                            \
        __builtin_unreachable(); \
    } while (0)

#define IWRAM_CODE __attribute__((section(".iwram"), long_call))
#define ARM_TARGET __attribute__((target("arm")))

/**
 * @brief Utility function to set bits at a certain location
 * 
 * Target gets the bits from shift - shift + length exclusive set to value
 */
void LostGBA_SetBits16(u16 *target, u16 value, u16 length, u16 shift);

/**
 * @brief Returns a number with the first n bits set to 1
 */
inline u16 LostGBA_AllOnes16(int length)
{
    return (1 << length) - 1;
}
