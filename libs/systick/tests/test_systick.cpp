#include "Mock_Systick.h"
#include "systick.h"
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace hal;

// =============================================================================
// Fixture: Systick with standard parameters (1MHz clock, 1kHz tick)
// =============================================================================

class SystickTest : public ::testing::Test {
public:
    Mock_Systick mock;

protected:
    SystickTest() : systick(0, 1'000'000, 1'000) {}

    void SetUp() override {
        ON_CALL(mock, enable())
            .WillByDefault(::testing::Invoke(&mock, &MockSystickImpl::enable));
        ON_CALL(mock, disable())
            .WillByDefault(::testing::Invoke(&mock, &MockSystickImpl::disable));
        ON_CALL(mock, acknowledge_irq())
            .WillByDefault(::testing::Invoke(&mock, &MockSystickImpl::acknowledge_irq));
        ON_CALL(mock, get_interval())
            .WillByDefault(::testing::Invoke(&mock, &MockSystickImpl::get_interval));
        ON_CALL(mock, get_count())
            .WillByDefault(::testing::Invoke(&mock, &MockSystickImpl::get_count));
        ON_CALL(mock, get_overflow())
            .WillByDefault(::testing::Invoke(&mock, &MockSystickImpl::get_overflow));
    }

    Systick systick;
};

// --- Construction & Initialization ---

TEST_F(SystickTest, IntervalMatchesClockDivision) {
    EXPECT_EQ(systick.get_interval(), 1000);
}

TEST_F(SystickTest, InitialOverflowIsZero) {
    EXPECT_EQ(systick.get_overflow(), 0);
}

TEST_F(SystickTest, InitialCountEqualsReloadValue) {
    EXPECT_EQ(systick.get_count(), 999);
}

// --- Enable / Disable ---

TEST_F(SystickTest, EnableSetsEnableBit) {
    systick.enable();
    SUCCEED();
}

TEST_F(SystickTest, DisableClearsEnableBit) {
    systick.disable();
    SUCCEED();
}

TEST_F(SystickTest, EnableThenDisableCycle) {
    systick.enable();
    systick.disable();
    SUCCEED();
}

TEST_F(SystickTest, MultipleEnableCallsAreSafe) {
    systick.enable();
    systick.enable();
    systick.enable();
    SUCCEED();
}

TEST_F(SystickTest, MultipleDisableCallsAreSafe) {
    systick.disable();
    systick.disable();
    systick.disable();
    SUCCEED();
}

// --- IRQ Acknowledgement ---

TEST_F(SystickTest, AcknowledgeIrqReadsCSR) {
    systick.acknowledge_irq();
    SUCCEED();
}

TEST_F(SystickTest, AcknowledgeIrqMultipleCallsSafe) {
    systick.acknowledge_irq();
    systick.acknowledge_irq();
    systick.acknowledge_irq();
    SUCCEED();
}

// --- Getters ---

TEST_F(SystickTest, GetIntervalIsConstant) {
    std::uint32_t first = systick.get_interval();
    std::uint32_t second = systick.get_interval();
    EXPECT_EQ(first, second);
}

TEST_F(SystickTest, GetCountIsNonNegative) {
    EXPECT_GE(systick.get_count(), 0u);
}

TEST_F(SystickTest, GetOverflowReturnsBitValue) {
    std::uint32_t overflow = systick.get_overflow();
    EXPECT_TRUE(overflow == 0 || overflow == 1);
}

// =============================================================================
// Fixture: Systick parameter combinations
// =============================================================================

class SystickParameterTest : public ::testing::Test {
public:
    Mock_Systick mock;
};

TEST_F(SystickParameterTest, StandardCortexM4_180MHz_1kHz) {
    Systick systick(0, 180'000'000, 1'000);
    EXPECT_EQ(systick.get_interval(), 180'000);
}

TEST_F(SystickParameterTest, StandardCortexM4_180MHz_10kHz) {
    Systick systick(0, 180'000'000, 10'000);
    EXPECT_EQ(systick.get_interval(), 18'000);
}

TEST_F(SystickParameterTest, LowClock_8MHz_1kHz) {
    Systick systick(0, 8'000'000, 1'000);
    EXPECT_EQ(systick.get_interval(), 8'000);
}

TEST_F(SystickParameterTest, HighTickRate_1MHz_100kHz) {
    Systick systick(0, 1'000'000, 100'000);
    EXPECT_EQ(systick.get_interval(), 10);
}

TEST_F(SystickParameterTest, MinimumDivider_EqualClockAndTick) {
    Systick systick(0, 1'000, 1'000);
    EXPECT_EQ(systick.get_interval(), 2);
}

TEST_F(SystickParameterTest, ReloadValueClampedTo24Bit) {
    Systick systick(0, 0xFFFF'FFFF, 1);
    EXPECT_LE(systick.get_interval(), 0x0100'0000U);
}

TEST_F(SystickParameterTest, LargeClockSmallTick) {
    Systick systick(0, 100'000'000, 1);
    EXPECT_LE(systick.get_interval(), 0x0100'0000U);
}

// =============================================================================
// Fixture: Systick with explicit base address (hardware register simulation)
// =============================================================================

class SystickHardwareTest : public ::testing::Test {
public:
    Mock_Systick mock;

protected:
    SystickHardwareTest()
        : base(reinterpret_cast<mmio::address_t>(&hw_regs[0]))
        , systick(base, 1'000'000, 1'000) {}

    void SetUp() override {
        ON_CALL(mock, enable())
            .WillByDefault(::testing::Invoke(&mock, &MockSystickImpl::enable));
        ON_CALL(mock, disable())
            .WillByDefault(::testing::Invoke(&mock, &MockSystickImpl::disable));
        ON_CALL(mock, acknowledge_irq())
            .WillByDefault(::testing::Invoke(&mock, &MockSystickImpl::acknowledge_irq));
        ON_CALL(mock, get_interval())
            .WillByDefault(::testing::Invoke(&mock, &MockSystickImpl::get_interval));
        ON_CALL(mock, get_count())
            .WillByDefault(::testing::Invoke(&mock, &MockSystickImpl::get_count));
        ON_CALL(mock, get_overflow())
            .WillByDefault(::testing::Invoke(&mock, &MockSystickImpl::get_overflow));
    }

    std::uint32_t hw_regs[4] = {0, 0, 0, 0};
    mmio::address_t base;
    Systick systick;
};

TEST_F(SystickHardwareTest, ConstructorSetsClockSourceBit) {
    EXPECT_TRUE((hw_regs[0] >> 2) & 1);
}

TEST_F(SystickHardwareTest, ConstructorSetsTickIntBit) {
    EXPECT_TRUE((hw_regs[0] >> 1) & 1);
}

TEST_F(SystickHardwareTest, ConstructorWritesReloadValue) {
    EXPECT_EQ(hw_regs[1], 999u);
}

TEST_F(SystickHardwareTest, ConstructorClearsCurrentValue) {
    EXPECT_EQ(hw_regs[2], 0u);
}

TEST_F(SystickHardwareTest, EnableSetsEnableBitInCSR) {
    systick.enable();
    EXPECT_TRUE(hw_regs[0] & 1);
}

TEST_F(SystickHardwareTest, DisableClearsEnableBitInCSR) {
    systick.enable();
    systick.disable();
    EXPECT_FALSE(hw_regs[0] & 1);
}

TEST_F(SystickHardwareTest, EnablePreservesOtherBits) {
    systick.enable();
    EXPECT_TRUE((hw_regs[0] >> 1) & 1);
    EXPECT_TRUE((hw_regs[0] >> 2) & 1);
}

TEST_F(SystickHardwareTest, DisablePreservesOtherBits) {
    systick.enable();
    systick.disable();
    EXPECT_TRUE((hw_regs[0] >> 1) & 1);
    EXPECT_TRUE((hw_regs[0] >> 2) & 1);
}

TEST_F(SystickHardwareTest, GetCountReturnsReloadMinusCurrent) {
    hw_regs[1] = 999;
    hw_regs[2] = 500;
    EXPECT_EQ(systick.get_count(), 499);
}

TEST_F(SystickHardwareTest, GetOverflowReadsCountflagBit) {
    hw_regs[0] |= (1u << 16);
    EXPECT_EQ(systick.get_overflow(), 1);
}

TEST_F(SystickHardwareTest, GetOverflowZeroWhenCountflagClear) {
    hw_regs[0] &= ~(1u << 16);
    EXPECT_EQ(systick.get_overflow(), 0);
}

// =============================================================================
// Destructor behavior
// =============================================================================

class SystickLifecycleTest : public ::testing::Test {
public:
    Mock_Systick mock;

protected:
    std::uint32_t hw_regs[4] = {0, 0, 0, 0};
};

TEST_F(SystickLifecycleTest, DestructorDisablesTimer) {
    mmio::address_t base = reinterpret_cast<mmio::address_t>(&hw_regs[0]);
    {
        Systick systick(base, 1'000'000, 1'000);
        systick.enable();
        EXPECT_TRUE(hw_regs[0] & 1);
    }
    EXPECT_FALSE(hw_regs[0] & 1);
}
