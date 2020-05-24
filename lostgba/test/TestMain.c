#include <lostgba/test/Test.h>

#include <lostgba/Interrupt.h>
#include <lostgba/SystemCalls.h>

#include <string.h>

struct RegisteredTest
{
    LostGBA_TestMethod testMethod;
    const char *testName;
};

#define MAX_TESTS 256

static struct RegisteredTest RegisteredTests[MAX_TESTS];
static int NumRegisteredTests;

void LostGBA_TestRegister(LostGBA_TestMethod testMethod, const char *testName)
{
    RegisteredTests[NumRegisteredTests].testMethod = testMethod;
    RegisteredTests[NumRegisteredTests].testName = testName;

    NumRegisteredTests++;
}

struct AgbPrintContext
{
    u16 request;
    u16 bank;
    u16 get;
    u16 put;
};

static volatile u16 *AgbPrintBuffer = (volatile u16 *)0x09fd0000;
static volatile struct AgbPrintContext *AgbPrintContext = (volatile struct AgbPrintContext *)0x09fe20f8;
static volatile u16 *AgbPrintProtect = (volatile u16 *)0x09fe2ffe;

static void agbPrintInit(void)
{
    *AgbPrintProtect = 0x20;
    AgbPrintContext->request = 0;
    AgbPrintContext->get = 0;
    AgbPrintContext->put = 0;
    AgbPrintContext->bank = 0xfd;
    *AgbPrintProtect = 0;
}

static void agbPutChar(char c)
{
    u16 data = AgbPrintBuffer[AgbPrintContext->put / 2];

    *AgbPrintProtect = 0x20;
    data = (AgbPrintContext->put % 2) ? (c << 8) | (data & 0xff) : (data & 0xff00) | c;
    AgbPrintBuffer[AgbPrintContext->put / 2] = data;
    AgbPrintContext->put++;
    *AgbPrintProtect = 0;
}

static void agbPrint(const char *text)
{
    agbPrintInit();
    while (*text)
    {
        agbPutChar(*text);
        text++;
    }

    asm volatile("swi 0xfa");
}

void LostGBA_TestAssertFailed(const char *testName, const char *fileName, int lineNumber, const char *message)
{
    agbPrint(testName);
    agbPrint(" failed with message ");
    agbPrint(message);

    while (1)
    {
    }
}

int main(void)
{
    Interrupt_Init();
    Interrupt_EnableType(InterruptType_VBlank);
    Interrupt_Enable();

    for (int i = 0; i < NumRegisteredTests; i++)
    {
        RegisteredTests[i].testMethod(RegisteredTests[i].testName);
    }

    agbPrint("All tests passed");
    while (1)
    {
        SystemCall_WaitForVBlank();
    }
}