#ifndef MMIO_ATOMIC_HPP
#define MMIO_ATOMIC_HPP

#include "mmio_register.hpp"
#include "mmio_irq_guard.hpp"

namespace mmio {

/// \brief Wraps a Register and provides atomic read‑modify‑write operations.
template <std::size_t Bits, typename Access = ReadWrite>
class AtomicRegister {
    using Reg = Register<Bits, Access>;
    using BaseType = typename RegisterTraits<Bits>::Type;

public:
    explicit AtomicRegister(Reg& reg) noexcept : reg_(reg) {}

    /// Atomically set bits (OR).
    void set_bits(BaseType mask) noexcept {
        critical_section([this, mask] {
            reg_ |= mask;
        });
    }

    /// Atomically clear bits (AND with complement).
    void clear_bits(BaseType mask) noexcept {
        critical_section([this, mask] {
            reg_ &= ~mask;
        });
    }

    /// Atomically toggle bits (XOR).
    void toggle_bits(BaseType mask) noexcept {
        critical_section([this, mask] {
            reg_ ^= mask;
        });
    }

    /// Atomically read‑modify‑write with a custom function.
    template <typename Func>
    void modify(Func&& func) noexcept {
        critical_section([this, &func] {
            BaseType val = reg_.read();
            func(val);
            reg_.write(val);
        });
    }

private:
    Reg& reg_;

    template <typename Func>
    static void critical_section(Func&& func) noexcept {
        IrqGuard guard;   // disables interrupts
        func();
    }
};

} // namespace mmio

#endif // MMIO_ATOMIC_HPP