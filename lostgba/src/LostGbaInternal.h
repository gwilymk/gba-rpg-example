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
 * @brief Utility function to set bits at a cirtain location
 * 
 * Same as LostGBA_SetBits16 except that it has a volatile target
 */
void LostGBA_SetVBits16(vu16 *target, u16 value, u16 length, u16 shift);

/**
 * @brief A version of memcpy that does 32-bit copies if possible and handles a volatile target
 */
void LostGBA_VMemCpy(volatile void *target, const void *src, int length);

/**
 * @brief Returns a number with the first n bits set to 1
 */
#define LostGBA_AllOnes16(length) ((1 << length) - 1)
