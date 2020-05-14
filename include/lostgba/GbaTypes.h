/**
 * @file GbaTypes.h
 * @brief Contains some simple definitions needed frequently while writing GBA games
 * 
 * The uint8_t etc. type names can get a bit long, so GbaTypes.h provides simple aliases to avoid
 * having to write these out in full.
 * 
 * Also provides useful macros needed to convince the C compiler to put stuff in memory the way you want.
 *
 * @defgroup GBA_TYPES GBA Integer types and memory macros
 * @{
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/** Unsigned 8 bit value */
typedef uint8_t u8;
/** Unsigned 16 bit value */
typedef uint16_t u16;
/** Unsigned 32 bit value */
typedef uint32_t u32;

/** Signed 8 bit value */
typedef int8_t s8;
/** Signed 16 bit value */
typedef int16_t s16;
/** Signed 32 bit value */
typedef int32_t s32;

/** Volatile unsigned 16 bit value */
typedef volatile u16 vu16;

/** 
 * @brief Tells the compiler that this must always be n-byte aligned
 * @param n The byte-alignment that this struct must always be in memory
 * 
 * Needed occasionally because the GBA is fussy about how things are aligned. Usage:
 * 
 * @code
 * struct MyStruct {
 *   u8 someField;
 * } LOSTGBA_ALIGN(4);
 * @endcode
 */
#define LOSTGBA_ALIGN(n) __attribute__((aligned(n)))

#define LOSTGBA_PACKED_ALIGN(n) __attribute__((packed, aligned(n)))

/** @} */