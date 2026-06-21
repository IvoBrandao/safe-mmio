#include "app_main.h"
#include "systick.h"
#include <gtest/gtest.h>

TEST(AppIntegrationTest, SystickConstructsWithDummyBase) {
    hal::Systick systick(0, 180'000'000U, 1'000U);
    EXPECT_EQ(systick.get_interval(), 180000);
}

TEST(AppIntegrationTest, SystickEnableDisableCycle) {
    hal::Systick systick(0, 180'000'000U, 1'000U);
    systick.enable();
    systick.disable();
}

TEST(AppIntegrationTest, SystickOverflowReadable) {
    hal::Systick systick(0, 180'000'000U, 1'000U);
    EXPECT_EQ(systick.get_overflow(), 0);
}
