/**
 * @file Print.h
 * @brief Handles printing to console with mgba
 *
 * @defgroup PRINTING Printing to the console with mgba
 * @{
 */
#pragma once

#ifdef LOSTGBA_MGBA_TARGET
/**
 * @brief Prints a line to the mgba console.
 * 
 * This will hang a regular GBA, so ensure this is only used in the emulator.
 * 
 * Only basic formatting is provided. The supported values are:
 * 
 * %% - prints a % sign
 * %s - prints a string
 * %c - prints a char
 * %d - prints an int
 */
void LostGBA_PrintLn(const char *fmt, ...)
    __attribute__((format(printf, 1, 2)));
#else
#define LostGBA_PrintLn(fmt, ...)
#endif

/** @} */