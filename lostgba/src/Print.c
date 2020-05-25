#ifdef LOSTGBA_MGBA_TARGET

#include <lostgba/Print.h>
#include <lostgba/GbaTypes.h>
#include <lostgba/SystemCalls.h>

#include <stdarg.h>

static volatile struct AgbPrintContext
{
    u16 request;
    u16 bank;
    u16 get;
    u16 put;
} *AgbPrintContext = (volatile struct AgbPrintContext *)0x09fe20f8;

static volatile u16 *AgbPrintBuffer = (volatile u16 *)0x09fd0000;
static volatile u16 *AgbPrintProtectReg = (volatile u16 *)0x09fe2ffe;

static void agbPrintUnprotect(void)
{
    *AgbPrintProtectReg = 0x20;
}

static void agbPrintProtect(void)
{
    *AgbPrintProtectReg = 0;
}

static void agbPrintInit(void)
{
    agbPrintUnprotect();
    AgbPrintContext->request = 0;
    AgbPrintContext->get = 0;
    AgbPrintContext->put = 0;
    AgbPrintContext->bank = 0xfd;
    agbPrintProtect();
}

static void agbPrintFlush(void)
{
    asm volatile("swi 0xfa");
}

static void agbPutChar(char c)
{
    u16 data = AgbPrintBuffer[AgbPrintContext->put / 2];

    agbPrintUnprotect();
    data = (AgbPrintContext->put % 2) ? (c << 8) | (data & 0xff) : (data & 0xff00) | c;
    AgbPrintBuffer[AgbPrintContext->put / 2] = data;
    AgbPrintContext->put++;
    agbPrintProtect();
}

static void agbPuts(const char *s)
{
    char c;
    while ((c = *(s++)))
    {
        agbPutChar(c);
    }
}

static void agbPutInt(s32 n)
{
    char buf[16] = {0};
    int i = 0;
    bool isNegative = n < 0;
    n = n < 0 ? -n : n;

    if (n < 10)
    {
        buf[i] = '0' + n;
        agbPuts(buf);
        return;
    }

    while (n != 0)
    {
        s32 rem;
        SystemCall_Divide(n, 10, &n, &rem);
        buf[i++] = rem + '0';
    }

    if (isNegative)
    {
        buf[i++] = '-';
    }

    int j = 0;
    i--;
    while (i > j)
    {
        char temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;

        i--;
        j++;
    }

    agbPuts(buf);
}

void LostGBA_PrintLn(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    agbPrintInit();

    char c;
    while ((c = *(fmt++)) != '\0')
    {
        if (c == '%')
        {
            switch (*fmt)
            {
            case '%':
                agbPutChar('%');
                break;
            case 's':
            {
                const char *arg = va_arg(args, const char *);
                agbPuts(arg);
                break;
            }
            case 'c':
            {
                char arg = (char)va_arg(args, int);
                agbPutChar(arg);
                break;
            }
            case 'd':
            {
                int arg = va_arg(args, int);
                agbPutInt(arg);
                break;
            }
            }

            fmt++;
        }
        else
        {
            agbPutChar(c);
        }
    }

    agbPrintFlush();
}

#endif