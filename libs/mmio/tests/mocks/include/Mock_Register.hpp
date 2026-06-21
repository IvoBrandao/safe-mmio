#ifndef MOCK_REGISTER_HPP
#define MOCK_REGISTER_HPP

#include "mmio.hpp"
#include <gmock/gmock.h>
#include <cstdint>

class MockInterfaceRegister {
public:
    virtual ~MockInterfaceRegister() = default;
    virtual std::uint32_t read() = 0;
    virtual void write(std::uint32_t value) = 0;
    virtual bool get_bit(std::size_t pos) = 0;
    virtual void set_bit(std::size_t pos, bool value) = 0;
    virtual mmio::address_t get_address() = 0;
    virtual void set_address(mmio::address_t addr) = 0;
    virtual bool is_valid() = 0;
};

class MockRegisterImpl : public MockInterfaceRegister {
protected:
    MockRegisterImpl();

public:
    virtual ~MockRegisterImpl();

    MOCK_METHOD(std::uint32_t, read, (), (override));
    MOCK_METHOD(void, write, (std::uint32_t value), (override));
    MOCK_METHOD(bool, get_bit, (std::size_t pos), (override));
    MOCK_METHOD(void, set_bit, (std::size_t pos, bool value), (override));
    MOCK_METHOD(mmio::address_t, get_address, (), (override));
    MOCK_METHOD(void, set_address, (mmio::address_t addr), (override));
    MOCK_METHOD(bool, is_valid, (), (override));

private:
    std::uint32_t dummy_register_;
    mmio::address_t base_address_;
};

using Mock_Register = ::testing::NiceMock<MockRegisterImpl>;
using StrictMock_Register = ::testing::StrictMock<MockRegisterImpl>;

class RegisterMockImplNotSetException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Register mock not instantiated in test fixture";
    }
};

#endif // MOCK_REGISTER_HPP
