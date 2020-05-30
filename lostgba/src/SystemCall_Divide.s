.include "AsmMacros.i"

@ void SystemCall_Divide(s32 numerator, s32 denominator, s32 *result, s32 *remainder)
@ Written in assembly because the C compiler does a horrible job of this
@
@ r0 = numerator
@ r1 = denominator
@ r2 = result
@ r3 = remainder
LostGBA_ThumbFunc SystemCall_Divide
    push {r3} @ r3 gets overwritten with ABS (Numerator / Denominator)
    swi 0x06 @ call the divide system call
    str r0, [r2]
    pop {r3} @ restore r3
    str r1, [r3]
    bx lr
LostGBA_EndThumbFunc SystemCall_Divide
