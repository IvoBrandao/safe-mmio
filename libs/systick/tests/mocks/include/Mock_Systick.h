#ifndef MOCK_SYSTICK_H_
#define MOCK_SYSTICK_H_

#include "systick.h"
#include <gmock/gmock.h>
#include <cstdint>
#include <exception>

class MockInterfaceSystick {
public:
    virtual ~MockInterfaceSystick() = default;
    virtual void enable() = 0;
    virtual void disable() = 0;
    virtual void acknowledge_irq() = 0;
    virtual std::uint32_t get_interval() = 0;
    virtual std::uint32_t get_count() = 0;
    virtual std::uint32_t get_overflow() = 0;
};

class MockSystickImpl : public MockInterfaceSystick {
protected:
    MockSystickImpl();

public:
    virtual ~MockSystickImpl();

    MOCK_METHOD(void, enable, (), (override));
    MOCK_METHOD(void, disable, (), (override));
    MOCK_METHOD(void, acknowledge_irq, (), (override));
    MOCK_METHOD(std::uint32_t, get_interval, (), (override));
    MOCK_METHOD(std::uint32_t, get_count, (), (override));
    MOCK_METHOD(std::uint32_t, get_overflow, (), (override));

private:
    std::uint32_t dummy_registers_[4];
    mmio::address_t base_address_;
};

using Mock_Systick = ::testing::NiceMock<MockSystickImpl>;
using StrictMock_Systick = ::testing::StrictMock<MockSystickImpl>;

class SystickMockImplNotSetException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Systick mock not instantiated in test fixture";
    }
};

#endif // MOCK_SYSTICK_H_
