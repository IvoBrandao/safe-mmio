#ifndef MMIO_REGISTER_HPP
#define MMIO_REGISTER_HPP

#include "mmio_core.hpp"
#include <cassert>
#include <cstdint>
#include <type_traits>

namespace mmio {

// ----------------------------------------------------------------------
// Register class
// ----------------------------------------------------------------------

/// \brief Models a memory‑mapped register of a given size and access policy.
/// \tparam Bits        Number of bits (8,16,32,64).
/// \tparam Access      One of ReadOnly, WriteOnly, or ReadWrite.
template <std::size_t Bits, typename Access = ReadWrite>
class Register {
    static_assert(Bits == 8 || Bits == 16 || Bits == 32 || Bits == 64,
                  "Register size must be 8, 16, 32, or 64 bits");

    using BaseType = typename RegisterTraits<Bits>::Type;

    template <typename A = Access>
    using enable_if_readable =
        typename std::enable_if<std::is_base_of<ReadOnly, A>::value, int>::type;

    template <typename A = Access>
    using enable_if_writeable =
        typename std::enable_if<std::is_base_of<WriteOnly, A>::value, int>::type;

public:
    /// \brief Default constructor – register has no address.
    constexpr Register() noexcept : raw_ptr_(nullptr) {}

    /// \brief Construct with a fixed address.
    constexpr explicit Register(address_t address) noexcept
        : raw_ptr_(reinterpret_cast<volatile BaseType*>(address)) {}

    // Non‑copyable (address should not be duplicated).
    Register(const Register&) = delete;
    Register& operator=(const Register&) = delete;

    // Move also deleted.
    Register(Register&&) = delete;
    Register& operator=(Register&&) = delete;

    /// \brief Read the register value (only for readable registers).
    template <typename A = Access, enable_if_readable<A> = 0>
    BaseType read() const noexcept {
        assert(raw_ptr_ != nullptr);
        BaseType val = *raw_ptr_;
        debug_print("read", get_address(), val);
        return val;
    }

    /// \brief Write a value (only for writeable registers).
    template <typename A = Access, enable_if_writeable<A> = 0>
    void write(BaseType value) noexcept {
        assert(raw_ptr_ != nullptr);
        *raw_ptr_ = value;
        debug_print("write", get_address(), value);
    }

    /// \brief Read single bit (only for readable).
    template <typename A = Access, enable_if_readable<A> = 0>
    bool get_bit(std::size_t pos) const noexcept {
        assert(pos < Bits);
        return (read() >> pos) & 1u;
    }

    /// \brief Write single bit (only for writeable).
    template <typename A = Access, enable_if_writeable<A> = 0>
    void set_bit(std::size_t pos, bool value) noexcept {
        assert(pos < Bits);
        BaseType val = read();
        if (value)
            val |= (BaseType{1} << pos);
        else
            val &= ~(BaseType{1} << pos);
        write(val);
    }

    /// \brief Get the memory address.
    constexpr address_t get_address() const noexcept {
        return reinterpret_cast<address_t>(raw_ptr_);
    }

    /// \brief Set the address (only allowed if the register is not yet used).
    void set_address(address_t addr) noexcept {
        raw_ptr_ = reinterpret_cast<volatile BaseType*>(addr);
    }

    bool is_valid() const noexcept {
        return raw_ptr_ != nullptr;
    }


    /// Conversion to BaseType – reads.
    template <typename A = Access, enable_if_readable<A> = 0>
    operator BaseType() const noexcept {
        return read();
    }

    /// Assignment – writes.
    template <typename A = Access, enable_if_writeable<A> = 0>
    Register& operator=(BaseType value) noexcept {
        write(value);
        return *this;
    }

    /// Prefix increment.
    template <typename A = Access, enable_if_writeable<A> = 0>
    BaseType operator++() noexcept {
        BaseType val = read();
        ++val;
        write(val);
        return val;
    }

    /// Postfix increment.
    template <typename A = Access, enable_if_writeable<A> = 0>
    BaseType operator++(int) noexcept {
        BaseType old = read();
        write(old + 1);
        return old;
    }

    /// Prefix decrement.
    template <typename A = Access, enable_if_writeable<A> = 0>
    BaseType operator--() noexcept {
        BaseType val = read();
        --val;
        write(val);
        return val;
    }

    /// Postfix decrement.
    template <typename A = Access, enable_if_writeable<A> = 0>
    BaseType operator--(int) noexcept {
        BaseType old = read();
        write(old - 1);
        return old;
    }

    /// Compound addition.
    template <typename A = Access, enable_if_writeable<A> = 0>
    Register& operator+=(BaseType rhs) noexcept {
        write(read() + rhs);
        return *this;
    }

    /// Compound subtraction.
    template <typename A = Access, enable_if_writeable<A> = 0>
    Register& operator-=(BaseType rhs) noexcept {
        write(read() - rhs);
        return *this;
    }

    /// Bitwise OR assignment.
    template <typename A = Access, enable_if_writeable<A> = 0>
    Register& operator|=(BaseType mask) noexcept {
        write(read() | mask);
        return *this;
    }

    /// Bitwise AND assignment.
    template <typename A = Access, enable_if_writeable<A> = 0>
    Register& operator&=(BaseType mask) noexcept {
        write(read() & mask);
        return *this;
    }

    /// Bitwise XOR assignment.
    template <typename A = Access, enable_if_writeable<A> = 0>
    Register& operator^=(BaseType mask) noexcept {
        write(read() ^ mask);
        return *this;
    }

    // ------------------------------------------------------------------------
    // Multi‑bit field proxy.
    // ------------------------------------------------------------------------

    class FieldProxy {
        Register& reg_;
        BaseType mask_;
        std::size_t shift_;

    public:
        FieldProxy(Register& reg, BaseType mask, std::size_t shift) noexcept
            : reg_(reg), mask_(mask), shift_(shift) {}

        /// Read the field.
        template <typename A = Access, enable_if_readable<A> = 0>
        operator BaseType() const noexcept {
            return (reg_.read() >> shift_) & mask_;
        }

        /// Write the field.
        template <typename A = Access, enable_if_writeable<A> = 0>
        FieldProxy& operator=(BaseType value) noexcept {
            assert((value & ~mask_) == 0);
            BaseType current = reg_.read();
            current &= ~(mask_ << shift_);
            current |= (value << shift_);
            reg_.write(current);
            return *this;
        }
    };

    /// \brief Access a multi‑bit field.
    /// \param mask  Bit mask of the field (e.g., 0b1111).
    /// \param shift Bit offset.
    FieldProxy field(BaseType mask, std::size_t shift) noexcept {
        return FieldProxy(*this, mask, shift);
    }

private:
    volatile BaseType* raw_ptr_;
};

} // namespace mmio

#endif // MMIO_REGISTER_HPP
