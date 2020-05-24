@ void LostGBA_VMemCpy32_Fast(volatile void *target, const void *src, int words)
@
@ Based very heavily on the implementation of memcpy32 from libtonc
@ Note that words MUST be > 0
@
@ Arguments:
@ r0, r1 = target, src
@ r2 = length (in words)
@ r0 and r1 are left pointing to the next word which would be written
@
@ Usage
@ r3 = length / 8 to get the number of 8 sections of words to move
@ r2 -> length % 8 to get the residual words to copy
@ r4 - r11 are used as scratch registers
.section .iwram, "ax", %progbits @ "ax" = allocatable and executable, %progbits = contains data
.arm
.align 2
.global LostGBA_VMemCpy32_Fast
LostGBA_VMemCpy32_Fast:
    lsr r3, r2, #3 @ r3 = r2 >> 3 (r3 = r2 / 8)
    and r2, r2, #7 @ r2 = r2 & 0b111 (r2 = r2 % 8)

    push {r4-r11} @ save the preserved registers according to the calling convention

.loopmemcpy32:
    ldmia r1!, {r4-r11} @ the clever bit. Copies memory to registers and stores. Increments r1 afterwards
    stmia r0!, {r4-r11} @ copies 8 32-bit words at a time :D

    subs r3, r3, #1 @ subtract 1 from r3
    bhi .loopmemcpy32 @ if r3 > 0, jump to loopmemcpy32

    @ we now have to copy r2 words. I'm not sure if it is quicker to write all the
    @ individual ldmia instructions, or to just do a basic loop. I've gone with a
    @ basic loop for now, but in future could consider trying to do all of these in 1
    @ instruction.

.loopmemcpy32_slow:
    @ for simplicity of writing the code here, we use the `hi` version of the ldmia and stmia instructions
    @ They won't execute if the subtraction goes negative
    subs r2, r2, #1 @ subtract 1 from r2
    ldmhiia r1!, {r4} @ copy *r1 to r4 and increment r1 by 4 but only if r2 > 0
    stmhiia r0!, {r4} @ store r4 to *r0 and increment r0 by 4 but only if r2 > 0
    bhi .loopmemcpy32_slow @ loop only if r2 > 0

    pop {r4-r11} @ return scratch registers to previous state
    bx lr @ return


@ void LostGBA_VMemCpy16(volatile void *target, const void *src, int length)
@
@ Based very heavily on the implementation of memcpy32 from libtonc
@ Note again that length must be > 0
@
@ Arguments:
@ r0, r1 = target, src
@ r2 = length (in bytes)
@
@ Will use the fast 32-bit version above internally but this handles non-word length (but not unaligned memory)
@ Both source and target must be word aligned, and length must be a multiple of 2 (half-word aligned)
@
@ TODO: Handle the case where source and target aren't word aligned
@ TODO: Handle the case where source xor target aren't word aligned
@ TODO: Handle the case where bytes % 2 != 0
.section .iwram, "ax", %progbits @ "ax" = allocatable and executable, %progbits = contains data
.arm
.align 2
.global LostGBA_VMemCpy16
LostGBA_VMemCpy16:
    @ we need to work out how many words we can copy using the fast version
    and r4, r2, #2 @ r4 = r2 & 0b10 the number of extra bytes we need to copy at the end (same as %4 but always even)
    lsr r2, r2, #2 @ r2 = r2 >> 2 (r2 = r2 / 4) so r2 is the number of words we need to copy

    @ everything is set up to call the fast version
    bl LostGBA_VMemCpy32_Fast

    @ now we only need to do the last half word if necessary
    cmp r4, #0 @ compare r4 with 0
    ldrneh r3, [r1, r4] @ r3 = r1 with r4 bytes offset, only if r4 is not 0
    strneh r3, [r0, r4] @ r0 with r4 bytes offset = r3 only if r4 is not 0
    
    bx lr
