#include <lostgba/Interrupt.h>
#include <lostgba/Graphics.h>

#include "LostGbaInternal.h"

static vu16 *Interrupt_enabledInterrupts = (vu16 *)0x04000200;          // REG_IE
static vu16 *Interrupt_acknowledgedInterrupts = (vu16 *)0x04000202;     // REG_IF
static vu16 *Interrupt_acknowledgedInterruptsBios = (vu16 *)0x03007ff8; // REG_IFBIOS
static vu16 *Interrupt_shouldThereBeInterrupts = (vu16 *)0x04000208;    // REG_IME

typedef void (*voidFnPtr)(void);
static voidFnPtr *Interrupt_isrMainRegister = (voidFnPtr *)0x03007ffc;

IWRAM_CODE ARM_TARGET static void Interrupt_interruptServiceRoutineMain(void)
{
    u32 irqs = *Interrupt_enabledInterrupts & *Interrupt_acknowledgedInterrupts;

    // Do something clever with them

    *Interrupt_acknowledgedInterrupts = irqs;
    *Interrupt_acknowledgedInterruptsBios |= irqs;
}

void Interrupt_Init(void)
{
    *Interrupt_isrMainRegister = &Interrupt_interruptServiceRoutineMain;
}

void Interrupt_EnableType(enum InterruptType interruptType)
{
    switch (interruptType)
    {
    case InterruptType_VBlank:
        Graphics_SetVBlankInterrupt(true);
        break;
    default: // TODO: the rest of these
        break;
    }

    *Interrupt_enabledInterrupts |= (1 << interruptType);
}

void Interrupt_Enable(void)
{
    *Interrupt_shouldThereBeInterrupts = 1;
}