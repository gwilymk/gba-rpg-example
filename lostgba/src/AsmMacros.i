.macro LostGBA_ArmFunc functionName:req
.section .iwram, "ax", %progbits @ "ax" = allocatable and executable, %progbits = contains data
.arm
.align 2
.global \functionName
.type \functionName, %function
.func \functionName
\functionName:
.endm

.macro LostGBA_EndArmFunc functionName:req
.pool
.size \functionName,.-\functionName
.endfunc
.endm

.macro LostGBA_ThumbFunc functionName:req
.section .text, "ax", %progbits @ "ax" = allocatable and executable, %progbits = contains data
.thumb_func
.code 16
.align 2
.global \functionName
.type \functionName, %function
.func \functionName
\functionName:
.endm

.macro LostGBA_EndThumbFunc functionName:req
.pool
.size \functionName,.-\functionName
.endfunc
.endm
