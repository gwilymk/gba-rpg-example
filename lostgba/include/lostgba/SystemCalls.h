/**
 * @file SystemCalls.h
 * @brief Handles GBA BIOS system calls elegantly-ish
 * 
 * @defgroup SYSTEM_CALLS System calls
 * @{
 */

#pragma once

#include "GbaTypes.h"

/** Halts the CUP until a VBlank occurs. Ensure that VBlank interrupts are enabled otherwise this will hang. */
void SystemCall_WaitForVBlank(void);

/** Performs a division and collects both the result and the remainder */
void SystemCall_Divide(s32 numerator, s32 denominator, s32 *result, s32 *remainder);

/** @} */