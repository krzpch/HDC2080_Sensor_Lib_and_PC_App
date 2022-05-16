#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

extern "C"
{
#include "HDC2080.h"
}

TEST_GROUP(HDC2080_TestGroup)
{
    void setup()
    {
    }

    void teardown()
    {
        mock().clear();
    }
};

TEST(HDC2080_TestGroup, HDC2080_Set_Temperature_Resolution_14bit)
{
    STRCMP_EQUAL("hello", "world");
    mock().checkExpectations();
}