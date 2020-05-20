/**
 * @file SystemCalls.h
 * @brief Handles GBA BIOS system calls elegantly-ish
 * 
 * @defgroup SYSTEM_CALLS System calls
 * @{
 */

#pragma once

/** Halts the CUP until a VBlank occurs. Ensure that VBlank interrupts are enabled otherwise this will hang. */
void SystemCall_WaitForVBlank(void);

/** @} */