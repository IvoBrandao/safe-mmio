#ifndef MMIO_IRQ_GUARD_HPP
#define MMIO_IRQ_GUARD_HPP

#include <cstdint>

namespace mmio {

/// \brief RAII class to disable and restore interrupts.
///        Platform‑specific implementation must be provided.
class IrqGuard {
public:
    IrqGuard() noexcept {
#if defined(__arm__) || defined(__thumb__)
        __asm volatile("MRS %0, PRIMASK" : "=r"(primask_));
        __asm volatile("CPSID i" ::: "memory");
#endif
    }

    ~IrqGuard() noexcept {
#if defined(__arm__) || defined(__thumb__)
        __asm volatile("MSR PRIMASK, %0" ::"r"(primask_) : "memory");
#endif
    }

    IrqGuard(const IrqGuard&) = delete;
    IrqGuard& operator=(const IrqGuard&) = delete;

private:
    std::uint32_t primask_ = 0;
};

} // namespace mmio

#endif // MMIO_IRQ_GUARD_HPP
