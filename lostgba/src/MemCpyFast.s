@ void LostGBA_VMemCpy32_Fast(volatile void *target, const void *src, int words)
@
@ Based very heavily on the implementation of memcpy32 from libtonc
@
@ Arguments:
@ r0, r1 = target, src
@ r2 = length (in words)
@ r0 and r1 are left pointing to the next word which would be written
@
@ Usage
@ r3 = length / 8 to get the number of 8 sections of words to move
@ r2 -> length % 8 to get the residual words to copy
@ r5 - r12 are used as scratch registers
@
@ preserves r4
.section .iwram, "ax", %progbits @ "ax" = allocatable and executable, %progbits = contains data
.arm
.align 2
.global LostGBA_VMemCpy32_Fast
.type LostGBA_VMemCpy32_Fast STT_FUNC
LostGBA_VMemCpy32_Fast:
    lsrs r3, r2, #3 @ r3 = r2 >> 3 (r3 = r2 / 8). Set flag register on result (used below)
    and r2, r2, #7 @ r2 = r2 & 0b111 (r2 = r2 % 8)
    beq .loopmemcpy32_slow @ if result above is = 0, then jump straight to the slow version

    push {r5-r11} @ save the preserved registers according to the calling convention

.loopmemcpy32:
    ldmia r1!, {r5-r12} @ the clever bit. Copies memory to registers and stores. Increments r1 afterwards
    stmia r0!, {r5-r12} @ copies 8 32-bit words at a time :D

    subs r3, r3, #1 @ subtract 1 from r3
    bne .loopmemcpy32 @ if r3 > 0, jump to loopmemcpy32

    @ we now have to copy r2 words. I'm not sure if it is quicker to write all the
    @ individual ldmia instructions, or to just do a basic loop. I've gone with a
    @ basic loop for now, but in future could consider trying to do all of these in 1
    @ instruction.

    pop {r5-r11} @ return scratch registers to previous state

.loopmemcpy32_slow:
    @ for simplicity of writing the code here, we use the `hi` version of the ldmia and stmia instructions
    @ They won't execute if the subtraction goes negative
    subs r2, r2, #1 @ subtract 1 from r2
    ldmplia r1!, {r3} @ copy *r1 to r3 and increment r1 by 4 but only if r2 > 0 (i.e. the subtraction resulted non-negative)
    stmplia r0!, {r3} @ store r3 to *r0 and increment r0 by 4 but only if r2 > 0
    bpl .loopmemcpy32_slow @ loop only if r2 > 0

    bx lr @ return


@ void LostGBA_VMemCpy16(volatile void *target, const void *src, int length)
@
@ Based very heavily on the implementation of memcpy32 from libtonc
@ Note that length must be > 0
@
@ Arguments:
@ r0, r1 = target, src
@ r2 = length (in bytes)
@
@ Uses r5 for scratch values
@
@ Will use the fast 32-bit version above internally but this handles non-word length (but not unaligned memory)
@ Both source and target must be word aligned, and length must be a multiple of 2 (half-word aligned)
@
@ TODO: Handle the case where bytes % 2 != 0
.section .rom, "ax", %progbits @ "ax" = allocatable and executable, %progbits = contains data
.thumb
.code 16
.align 2
.global LostGBA_VMemCpy
.type LostGBA_VMemCpy STT_FUNC
LostGBA_VMemCpy:
    push {r4, r5, lr} @ save return value on the stack

    @ First check if length = 0
    cmp r2, #0
    beq .finishedCopying

    @ Check that the source and target are aligned. Logical or and check that the lower
    @ 2 bits are 0
    mov r4, r0 @ r4 = r0
    orr r4, r1 @ r4 = r4 | r1 (= r0 | r1)
    ldr r5, =#2
    and r4, r5 @ r4 = r4 & 0b10
    bne .unalignedSourceOrTarget @ unaligned if not 0

.alignedSourceAndTarget:
    @ we need to work out how many words we can copy using the fast version
    mov r4, r2 @ r4 = r2
    ldr r5, =#2
    and r4, r5 @ r4 = r4 @ 0b10 = length & 4 = number of half words to copy after copying words
    lsr r2, #2 @ r2 = r2 >> 2 (r2 = r2 / 4) so r2 is the number of words we need to copy

    @ everything is set up to call the fast version
    ldr r3, =LostGBA_VMemCpy32_Fast
    bl _call_via_r3 @ don't need to save r4 because that is preserved
    
    cmp r4, #0 @ compare r4 with 0 (the number of extra bits)
    beq .finishedCopying

    @ now we only need to do the last half word if necessary
    ldrh r3, [r1] @ r3 = *r1 (half word)
    strh r3, [r0] @ *r0 = r3 (half word)
    
.finishedCopying:
    pop {r4}
    pop {r5}
    pop {r3} @ restore scratch register and return pointer
    bx r3 @ return

.unalignedSourceOrTarget:
    mov r5, r0 @ r5 = r0
    eor r5, r1 @ r5 = r5 ^ r1 (= r0 ^ r1)
    ldr r3, =#2
    and r5, r3 @ r5 &= 0b10. Check unaligned
    bne .unalignedSourceXorTarget @ if above != 0, then we need to do halfword copies

    ldrh r3, [r1] @ copy the value of *r1 to *r0
    strh r3, [r0]
    add r0, #2 @ we've already copied the first half-word now
    add r1, #2
    sub r2, #2 @ and so decrement the length
    b .alignedSourceAndTarget @ source and target are now aligned

.unalignedSourceXorTarget:
    @ this just has to be half word copies the whole way along unfortunately. Try not to do this IRL
    @ the copying happens backwards
    sub r2, #2 @ r2 -= 2 @ we know that r2 is not 0

.unalignedSourceXorTargetLoop:
    ldrh r3, [r1, r2] @ *(r0 + r2) = *(r1 + r2)
    strh r3, [r0, r2]
    sub r2, #2 @ r2 -= 2
    bpl .unalignedSourceXorTargetLoop @ loop if r2 is now negative

    pop {r4}
    pop {r5}
    pop {r3} @ restore scratch register and return pointer
    bx r3 @ return
