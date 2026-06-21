#include "Mock_Register.hpp"
#include "mmio.hpp"
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace mmio;

// =============================================================================
// Fixture: 32-bit ReadWrite Register
// =============================================================================

class Register32Test : public ::testing::Test {
public:
    Mock_Register mock;

protected:
    Register32Test()
        : reg(reinterpret_cast<address_t>(&storage)) {}

    void SetUp() override {
        ON_CALL(mock, read())
            .WillByDefault(::testing::Invoke(&mock, &MockRegisterImpl::read));
        ON_CALL(mock, write(::testing::_))
            .WillByDefault(::testing::Invoke(&mock, &MockRegisterImpl::write));
        ON_CALL(mock, get_bit(::testing::_))
            .WillByDefault(::testing::Invoke(&mock, &MockRegisterImpl::get_bit));
        ON_CALL(mock, set_bit(::testing::_, ::testing::_))
            .WillByDefault(::testing::Invoke(&mock, &MockRegisterImpl::set_bit));
        ON_CALL(mock, get_address())
            .WillByDefault(::testing::Invoke(&mock, &MockRegisterImpl::get_address));
        ON_CALL(mock, set_address(::testing::_))
            .WillByDefault(::testing::Invoke(&mock, &MockRegisterImpl::set_address));
        ON_CALL(mock, is_valid())
            .WillByDefault(::testing::Invoke(&mock, &MockRegisterImpl::is_valid));
    }

    std::uint32_t storage = 0;
    Register<32, ReadWrite> reg;
};

// --- Basic Read/Write ---

TEST_F(Register32Test, WriteStoresValue) {
    reg.write(0xDEADBEEF);
    EXPECT_EQ(storage, 0xDEADBEEF);
}

TEST_F(Register32Test, ReadReturnsStoredValue) {
    storage = 0xCAFEBABE;
    EXPECT_EQ(reg.read(), 0xCAFEBABE);
}

TEST_F(Register32Test, AssignmentOperatorWrites) {
    reg = 0x12345678;
    EXPECT_EQ(storage, 0x12345678);
}

TEST_F(Register32Test, ImplicitConversionReads) {
    storage = 0xABCD;
    std::uint32_t val = reg;
    EXPECT_EQ(val, 0xABCD);
}

TEST_F(Register32Test, WriteZero) {
    storage = 0xFFFFFFFF;
    reg = 0;
    EXPECT_EQ(storage, 0u);
}

TEST_F(Register32Test, WriteMaxValue) {
    reg = 0xFFFFFFFF;
    EXPECT_EQ(storage, 0xFFFFFFFF);
}

// --- Bitwise OR ---

TEST_F(Register32Test, OrAssignmentSetsTargetBits) {
    reg = 0xAAAA0000;
    reg |= 0x00005555;
    EXPECT_EQ(storage, 0xAAAA5555);
}

TEST_F(Register32Test, OrWithZeroNoChange) {
    reg = 0x1234;
    reg |= 0;
    EXPECT_EQ(storage, 0x1234u);
}

TEST_F(Register32Test, OrWithAllOnesSetsFull) {
    reg = 0;
    reg |= 0xFFFFFFFF;
    EXPECT_EQ(storage, 0xFFFFFFFF);
}

// --- Bitwise AND ---

TEST_F(Register32Test, AndAssignmentClearsBits) {
    reg = 0xFFFFFFFF;
    reg &= 0x00FF00FF;
    EXPECT_EQ(storage, 0x00FF00FF);
}

TEST_F(Register32Test, AndWithAllOnesNoChange) {
    reg = 0xABCD;
    reg &= 0xFFFFFFFF;
    EXPECT_EQ(storage, 0xABCDu);
}

TEST_F(Register32Test, AndWithZeroClearsAll) {
    reg = 0xFFFFFFFF;
    reg &= 0;
    EXPECT_EQ(storage, 0u);
}

// --- Bitwise XOR ---

TEST_F(Register32Test, XorAssignmentTogglesBits) {
    reg = 0xAAAAAAAA;
    reg ^= 0xFFFFFFFF;
    EXPECT_EQ(storage, 0x55555555);
}

TEST_F(Register32Test, XorWithZeroNoChange) {
    reg = 0x1234;
    reg ^= 0;
    EXPECT_EQ(storage, 0x1234u);
}

TEST_F(Register32Test, XorWithSelfClearsAll) {
    reg = 0xDEAD;
    reg ^= 0xDEAD;
    EXPECT_EQ(storage, 0u);
}

// --- Arithmetic ---

TEST_F(Register32Test, AddAssignment) {
    reg = 100;
    reg += 50;
    EXPECT_EQ(storage, 150u);
}

TEST_F(Register32Test, AddAssignmentZero) {
    reg = 42;
    reg += 0;
    EXPECT_EQ(storage, 42u);
}

TEST_F(Register32Test, SubAssignment) {
    reg = 100;
    reg -= 30;
    EXPECT_EQ(storage, 70u);
}

TEST_F(Register32Test, SubAssignmentZero) {
    reg = 42;
    reg -= 0;
    EXPECT_EQ(storage, 42u);
}

// --- Increment ---

TEST_F(Register32Test, PrefixIncrementReturnsNewValue) {
    reg = 10;
    std::uint32_t val = ++reg;
    EXPECT_EQ(val, 11);
    EXPECT_EQ(storage, 11u);
}

TEST_F(Register32Test, PostfixIncrementReturnsOldValue) {
    reg = 10;
    std::uint32_t old = reg++;
    EXPECT_EQ(old, 10);
    EXPECT_EQ(storage, 11u);
}

TEST_F(Register32Test, IncrementFromZero) {
    reg = 0;
    ++reg;
    EXPECT_EQ(storage, 1u);
}

// --- Decrement ---

TEST_F(Register32Test, PrefixDecrementReturnsNewValue) {
    reg = 10;
    std::uint32_t val = --reg;
    EXPECT_EQ(val, 9);
    EXPECT_EQ(storage, 9u);
}

TEST_F(Register32Test, PostfixDecrementReturnsOldValue) {
    reg = 10;
    std::uint32_t old = reg--;
    EXPECT_EQ(old, 10);
    EXPECT_EQ(storage, 9u);
}

TEST_F(Register32Test, DecrementWrapsAround) {
    reg = 0;
    --reg;
    EXPECT_EQ(storage, 0xFFFFFFFF);
}

// --- Single Bit Access ---

TEST_F(Register32Test, SetBitHighAtPosition0) {
    reg = 0;
    reg.set_bit(0, true);
    EXPECT_EQ(storage, 1u);
}

TEST_F(Register32Test, SetBitHighAtPosition31) {
    reg = 0;
    reg.set_bit(31, true);
    EXPECT_EQ(storage, 0x80000000);
}

TEST_F(Register32Test, SetBitLowClearsOnly) {
    reg = 0xFF;
    reg.set_bit(0, false);
    EXPECT_EQ(storage, 0xFE);
}

TEST_F(Register32Test, SetBitLowPreservesOthers) {
    reg = 0xFF;
    reg.set_bit(4, false);
    EXPECT_EQ(storage, 0xEF);
}

TEST_F(Register32Test, GetBitReturnsTrue) {
    reg = 0x80;
    EXPECT_TRUE(reg.get_bit(7));
}

TEST_F(Register32Test, GetBitReturnsFalse) {
    reg = 0x80;
    EXPECT_FALSE(reg.get_bit(0));
}

TEST_F(Register32Test, GetBitAllPositions) {
    reg = 0xAAAAAAAA;
    for (std::size_t i = 0; i < 32; ++i) {
        EXPECT_EQ(reg.get_bit(i), (i % 2) == 1);
    }
}

TEST_F(Register32Test, SetBitMultiplePositions) {
    reg = 0;
    reg.set_bit(0, true);
    reg.set_bit(8, true);
    reg.set_bit(16, true);
    reg.set_bit(24, true);
    EXPECT_EQ(storage, 0x01010101);
}

// --- Field Proxy ---

TEST_F(Register32Test, FieldProxyWriteIsolatesField) {
    reg = 0xFFFFFFFF;
    auto f = reg.field(0xF, 8);
    f = 0x5;
    EXPECT_EQ((storage >> 8) & 0xF, 0x5u);
    EXPECT_EQ(storage & 0xFF, 0xFF);
    EXPECT_EQ((storage >> 12), 0xFFFFF);
}

TEST_F(Register32Test, FieldProxyReadIsolatesField) {
    reg = 0x00ABCD00;
    auto f = reg.field(0xFF, 8);
    EXPECT_EQ(static_cast<std::uint32_t>(f), 0xCD);
}

TEST_F(Register32Test, FieldProxyWriteZero) {
    reg = 0xFFFFFFFF;
    auto f = reg.field(0xFF, 16);
    f = 0;
    EXPECT_EQ(storage, 0xFF00FFFF);
}

TEST_F(Register32Test, FieldProxyWriteMax) {
    reg = 0;
    auto f = reg.field(0xFF, 24);
    f = 0xFF;
    EXPECT_EQ(storage, 0xFF000000);
}

TEST_F(Register32Test, MultipleFieldsNoInterference) {
    reg = 0;
    auto low = reg.field(0xF, 0);
    auto mid = reg.field(0xF, 4);
    auto high = reg.field(0xF, 8);
    low = 0xA;
    mid = 0xB;
    high = 0xC;
    EXPECT_EQ(storage, 0xCBA);
}

// --- Address Management ---

TEST_F(Register32Test, GetAddressReturnsConstructionValue) {
    EXPECT_EQ(reg.get_address(), reinterpret_cast<address_t>(&storage));
}

TEST_F(Register32Test, IsValidWhenConstructedWithAddress) {
    EXPECT_TRUE(reg.is_valid());
}

TEST_F(Register32Test, DefaultConstructedIsInvalid) {
    Register<32> r;
    EXPECT_FALSE(r.is_valid());
}

TEST_F(Register32Test, SetAddressMakesValid) {
    Register<32> r;
    r.set_address(reinterpret_cast<address_t>(&storage));
    EXPECT_TRUE(r.is_valid());
}

TEST_F(Register32Test, SetAddressAllowsAccess) {
    std::uint32_t other = 77;
    Register<32> r;
    r.set_address(reinterpret_cast<address_t>(&other));
    EXPECT_EQ(r.read(), 77u);
}

// =============================================================================
// Fixture: 8-bit Register
// =============================================================================

class Register8Test : public ::testing::Test {
public:
    Mock_Register mock;

protected:
    Register8Test()
        : reg(reinterpret_cast<address_t>(&storage)) {}

    std::uint8_t storage = 0;
    Register<8, ReadWrite> reg;
};

TEST_F(Register8Test, WriteAndRead) {
    reg = 0xAB;
    EXPECT_EQ(reg.read(), 0xAB);
}

TEST_F(Register8Test, MaxValue) {
    reg = 0xFF;
    EXPECT_EQ(storage, 0xFF);
}

TEST_F(Register8Test, SetBitHigh) {
    reg = 0;
    reg.set_bit(7, true);
    EXPECT_EQ(storage, 0x80);
}

TEST_F(Register8Test, OrAssignment) {
    reg = 0x0F;
    reg |= 0xF0;
    EXPECT_EQ(storage, 0xFF);
}

TEST_F(Register8Test, AndAssignment) {
    reg = 0xFF;
    reg &= 0x0F;
    EXPECT_EQ(storage, 0x0F);
}

TEST_F(Register8Test, IncrementDecrement) {
    reg = 5;
    ++reg;
    EXPECT_EQ(storage, 6);
    --reg;
    EXPECT_EQ(storage, 5);
}

// =============================================================================
// Fixture: 16-bit Register
// =============================================================================

class Register16Test : public ::testing::Test {
public:
    Mock_Register mock;

protected:
    Register16Test()
        : reg(reinterpret_cast<address_t>(&storage)) {}

    std::uint16_t storage = 0;
    Register<16, ReadWrite> reg;
};

TEST_F(Register16Test, WriteAndRead) {
    reg = 0xBEEF;
    EXPECT_EQ(reg.read(), 0xBEEF);
}

TEST_F(Register16Test, MaxValue) {
    reg = 0xFFFF;
    EXPECT_EQ(storage, 0xFFFF);
}

TEST_F(Register16Test, FieldProxy) {
    reg = 0;
    auto f = reg.field(0x1F, 5);
    f = 0x15;
    EXPECT_EQ(static_cast<std::uint16_t>(f), 0x15);
}

TEST_F(Register16Test, BitAccess) {
    reg = 0;
    reg.set_bit(15, true);
    EXPECT_EQ(storage, 0x8000);
    EXPECT_TRUE(reg.get_bit(15));
}

// =============================================================================
// Fixture: 64-bit Register
// =============================================================================

class Register64Test : public ::testing::Test {
public:
    Mock_Register mock;

protected:
    Register64Test()
        : reg(reinterpret_cast<address_t>(&storage)) {}

    std::uint64_t storage = 0;
    Register<64, ReadWrite> reg;
};

TEST_F(Register64Test, WriteAndRead) {
    reg = 0xDEADBEEFCAFEBABE;
    EXPECT_EQ(reg.read(), 0xDEADBEEFCAFEBABE);
}

TEST_F(Register64Test, SetBitHighBit63) {
    reg = 0;
    reg.set_bit(63, true);
    EXPECT_EQ(storage, 0x8000000000000000ULL);
}

TEST_F(Register64Test, OrAssignment) {
    reg = 0x00000000FFFFFFFF;
    reg |= 0xFFFFFFFF00000000;
    EXPECT_EQ(storage, 0xFFFFFFFFFFFFFFFF);
}

TEST_F(Register64Test, FieldProxyHighBits) {
    reg = 0;
    auto f = reg.field(0xFF, 56);
    f = 0xAB;
    EXPECT_EQ(storage, 0xAB00000000000000ULL);
}

// =============================================================================
// Access Policy Tests
// =============================================================================

class ReadOnlyRegisterTest : public ::testing::Test {
public:
    Mock_Register mock;

protected:
    ReadOnlyRegisterTest()
        : reg(reinterpret_cast<address_t>(&storage)) {}

    std::uint32_t storage = 0x12345678;
    Register<32, ReadOnly> reg;
};

TEST_F(ReadOnlyRegisterTest, ReadReturnsValue) {
    EXPECT_EQ(reg.read(), 0x12345678);
}

TEST_F(ReadOnlyRegisterTest, GetBitWorks) {
    EXPECT_TRUE(reg.get_bit(3));
    EXPECT_FALSE(reg.get_bit(0));
}

TEST_F(ReadOnlyRegisterTest, ImplicitConversion) {
    std::uint32_t val = reg;
    EXPECT_EQ(val, 0x12345678);
}

class WriteOnlyRegisterTest : public ::testing::Test {
public:
    Mock_Register mock;

protected:
    WriteOnlyRegisterTest()
        : reg(reinterpret_cast<address_t>(&storage)) {}

    std::uint32_t storage = 0;
    Register<32, WriteOnly> reg;
};

TEST_F(WriteOnlyRegisterTest, WriteStoresValue) {
    reg.write(0xABCD);
    EXPECT_EQ(storage, 0xABCD);
}

TEST_F(WriteOnlyRegisterTest, AssignmentOperator) {
    reg = 0x9999;
    EXPECT_EQ(storage, 0x9999);
}

// =============================================================================
// DynamicRegisterBlock Tests
// =============================================================================

class DynamicBlockTest : public ::testing::Test {
public:
    Mock_Register mock;

protected:
    using Block = DynamicRegisterBlock<
        at_offset<0x00, 32, ReadWrite>,
        at_offset<0x04, 32, ReadWrite>,
        at_offset<0x08, 32, ReadOnly>,
        at_offset<0x0C, 32, ReadWrite>
    >;

    DynamicBlockTest()
        : block(reinterpret_cast<address_t>(&regs[0])) {}

    std::uint32_t regs[4] = {0, 0, 0, 0};
    Block block;
};

TEST_F(DynamicBlockTest, WriteFirstRegister) {
    block.get<0>() = 0xAAAA;
    EXPECT_EQ(regs[0], 0xAAAA);
}

TEST_F(DynamicBlockTest, WriteSecondRegister) {
    block.get<1>() = 0xBBBB;
    EXPECT_EQ(regs[1], 0xBBBB);
}

TEST_F(DynamicBlockTest, WriteFourthRegister) {
    block.get<3>() = 0xDDDD;
    EXPECT_EQ(regs[3], 0xDDDD);
}

TEST_F(DynamicBlockTest, ReadThirdRegister) {
    regs[2] = 0xCCCC;
    EXPECT_EQ(block.get<2>().read(), 0xCCCC);
}

TEST_F(DynamicBlockTest, RegistersAreIndependent) {
    block.get<0>() = 0x1111;
    block.get<1>() = 0x2222;
    block.get<3>() = 0x4444;
    EXPECT_EQ(regs[0], 0x1111);
    EXPECT_EQ(regs[1], 0x2222);
    EXPECT_EQ(regs[3], 0x4444);
}

TEST_F(DynamicBlockTest, IsValidAfterConstruction) {
    EXPECT_TRUE(block.is_valid());
}

TEST_F(DynamicBlockTest, DefaultConstructedIsInvalid) {
    Block empty;
    EXPECT_FALSE(empty.is_valid());
}

TEST_F(DynamicBlockTest, SetBaseAddressMakesValid) {
    Block late;
    EXPECT_FALSE(late.is_valid());
    late.set_base_address(reinterpret_cast<address_t>(&regs[0]));
    EXPECT_TRUE(late.is_valid());
}

TEST_F(DynamicBlockTest, LateInitAllowsAccess) {
    Block late;
    late.set_base_address(reinterpret_cast<address_t>(&regs[0]));
    late.get<0>() = 42;
    EXPECT_EQ(regs[0], 42u);
}

TEST_F(DynamicBlockTest, ResetBaseAddress) {
    std::uint32_t other_regs[4] = {0, 0, 0, 0};
    block.set_base_address(reinterpret_cast<address_t>(&other_regs[0]));
    block.get<0>() = 99;
    EXPECT_EQ(other_regs[0], 99u);
    EXPECT_EQ(regs[0], 0u);
}

// =============================================================================
// Peripheral Wrapper Tests
// =============================================================================

class PeripheralTest : public ::testing::Test {
public:
    Mock_Register mock;

protected:
    using Block = DynamicRegisterBlock<
        at_offset<0x00, 32, ReadWrite>,
        at_offset<0x04, 32, ReadWrite>
    >;
    using Periph = Peripheral<Block>;

    PeripheralTest()
        : periph(reinterpret_cast<address_t>(&regs[0])) {}

    std::uint32_t regs[2] = {0, 0};
    Periph periph;
};

TEST_F(PeripheralTest, RegsAccessWritesHardware) {
    periph.regs().get<0>() = 0xCAFE;
    EXPECT_EQ(regs[0], 0xCAFE);
}

TEST_F(PeripheralTest, RegsAccessReadsHardware) {
    regs[1] = 0xBEEF;
    EXPECT_EQ(periph.regs().get<1>().read(), 0xBEEF);
}

TEST_F(PeripheralTest, ConstRegsAccess) {
    regs[0] = 0x1234;
    const Periph& cperiph = periph;
    EXPECT_EQ(cperiph.regs().get<0>().read(), 0x1234);
}

// =============================================================================
// AtomicRegister Tests
// =============================================================================

class AtomicRegisterTest : public ::testing::Test {
public:
    Mock_Register mock;

protected:
    AtomicRegisterTest()
        : reg(reinterpret_cast<address_t>(&storage))
        , atomic(reg) {}

    std::uint32_t storage = 0;
    Register<32, ReadWrite> reg;
    AtomicRegister<32> atomic;
};

TEST_F(AtomicRegisterTest, SetBitsPerformsOr) {
    storage = 0x00FF;
    atomic.set_bits(0xFF00);
    EXPECT_EQ(storage, 0xFFFF);
}

TEST_F(AtomicRegisterTest, ClearBitsPerformsAndComplement) {
    storage = 0xFFFF;
    atomic.clear_bits(0x00FF);
    EXPECT_EQ(storage, 0xFF00);
}

TEST_F(AtomicRegisterTest, ToggleBitsPerformsXor) {
    storage = 0xAAAA;
    atomic.toggle_bits(0xFFFF);
    EXPECT_EQ(storage, 0x5555);
}

TEST_F(AtomicRegisterTest, ModifyCustomFunction) {
    storage = 0x1234;
    atomic.modify([](std::uint32_t& val) {
        val = (val & 0xFF00) | 0x0056;
    });
    EXPECT_EQ(storage, 0x1256);
}

TEST_F(AtomicRegisterTest, SetBitsFromZero) {
    storage = 0;
    atomic.set_bits(0x80000001);
    EXPECT_EQ(storage, 0x80000001);
}

TEST_F(AtomicRegisterTest, ClearAllBits) {
    storage = 0xFFFFFFFF;
    atomic.clear_bits(0xFFFFFFFF);
    EXPECT_EQ(storage, 0u);
}
