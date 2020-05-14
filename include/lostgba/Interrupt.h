/**
 * @file Interrupt.h
 * @brief Basic interrupts handling
 * 
 * @defgroup INTERRUPT Interrupt handling
 * @{
 */

#pragma once

#include "GbaTypes.h"

/**
 * @brief Must be called before doing anything with interrupts.
 * 
 * Sets up the internal interrupt service routine
 */
void Interrupt_Init(void);

/**
 * Possible interrupt types
 */
enum InterruptType
{
    InterruptType_VBlank, /**< Triggers on vblank. This will automatically call Graphics_SetVBlankInterrupt(true) for you */
    InterruptType_HBlank,
    InterruptType_VCount,
    InterruptType_Timer0,
    InterruptType_Timer1,
    InterruptType_Timer2,
    InterruptType_Timer3,
    InterruptType_Dma0,
    InterruptType_Dma1,
    InterruptType_Dma2,
    InterruptType_Dma3,
    InterruptType_Keypad,
    InterruptType_Cartridge
};

/**
 * @brief Enables a specific type of interrupt.
 * 
 * Check the InterruptType documentation for additional requirements. Note that no interrupts will be fired
 * until you call Interrupt_Enable()
 */
void Interrupt_EnableType(enum InterruptType interruptType);

/**
 * @brief Actually enables interrupts.
 * 
 * Until this is called, no interrupts will be fired at all
 */
void Interrupt_Enable(void);

/** @} */