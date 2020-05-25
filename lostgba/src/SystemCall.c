#include <lostgba/SystemCalls.h>

#ifdef __thumb__
#define swi_call(x)                                 \
    do                                              \
    {                                               \
        asm volatile("swi\t" #x ::                  \
                         : "r0", "r1", "r2", "r3"); \
    } while (0)
#else
#define swi_call(x)                                 \
    do                                              \
    {                                               \
        asm volatile("swi\t" #x "<<16" ::           \
                         : "r0", "r1", "r2", "r3"); \
    } while (0)
#endif

void SystemCall_WaitForVBlank(void)
{
    swi_call(0x05);
}

void SystemCall_Divide(s32 numerator, s32 denominator, s32 *result, s32 *remainder)
{
    asm volatile(
        "mov r0, %2   \n"
        "mov r1, %3   \n"
        "swi 0x6      \n"
        "ldr r2, %0   \n"
        "str r0, [r2] \n"
        "ldr r2, %1   \n"
        "str r1, [r2] \n"
        : "=m"(result), "=m"(remainder)
        : "r"(numerator), "r"(denominator)
        : "r0", "r1", "r2", "r3");
}