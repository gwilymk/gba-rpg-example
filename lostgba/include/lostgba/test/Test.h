#pragma once

#define LostGBA_Test(testName)                                                       \
    static void LostGBA_Test_##__LINE__(const char *LostGBA_TestName);               \
    __attribute__((constructor)) static void LostGBA_Construct_Test_##__LINE__(void) \
    {                                                                                \
        LostGBA_TestRegister(&LostGBA_Test_##__LINE__, testName);                    \
    }                                                                                \
    static void LostGBA_Test_##__LINE__(const char *LostGBA_TestName)

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
