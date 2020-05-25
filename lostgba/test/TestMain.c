#include <lostgba/test/Test.h>

#include <lostgba/Interrupt.h>
#include <lostgba/SystemCalls.h>
#include <lostgba/Print.h>

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

void LostGBA_TestAssertFailed(const char *testName, const char *fileName, int lineNumber, const char *message)
{
    LostGBA_PrintLn("%s:%d, %s failed with message %s", fileName, lineNumber, testName, message);

    while (1)
    {
        SystemCall_WaitForVBlank();
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

    LostGBA_PrintLn("All tests passed!");
    while (1)
    {
        SystemCall_WaitForVBlank();
    }
}