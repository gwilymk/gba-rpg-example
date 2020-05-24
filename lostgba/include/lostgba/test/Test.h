#pragma once

#define LostGBA_Concat2__(a, b) a##b
#define LostGBA_Concat__(a, b) LostGBA_Concat2__(a, b)
#define LostGBA_TestName__ LostGBA_Concat__(LostGBA_Test_, __LINE__)

#define LostGBA_Test(testName)                                                                            \
    static void LostGBA_TestName__(const char *LostGBA_TestName);                                         \
    __attribute__((constructor)) static void LostGBA_Concat__(LostGBA_Register, LostGBA_TestName__)(void) \
    {                                                                                                     \
        LostGBA_TestRegister(&LostGBA_TestName__, testName);                                              \
    }                                                                                                     \
    static void LostGBA_TestName__(const char *LostGBA_TestName)

typedef void (*LostGBA_TestMethod)(const char *LostGBA_TestName);

void LostGBA_TestRegister(LostGBA_TestMethod testMethod, const char *testName);

void LostGBA_TestAssertFailed(const char *testName, const char *fileName, int lineNumber, const char *message);

#define LostGBA_Assert(test, message)                                                \
    do                                                                               \
    {                                                                                \
        if (!(test))                                                                 \
        {                                                                            \
            LostGBA_TestAssertFailed(LostGBA_TestName, __FILE__, __LINE__, message); \
        }                                                                            \
    } while (0)
