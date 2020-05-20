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