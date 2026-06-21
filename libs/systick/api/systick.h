#ifndef SYSTICK_H
#define SYSTICK_H

#include "mmio.hpp"
#include <cstdint>

namespace hal {

class Systick {
public:
    Systick(mmio::address_t base, std::uint32_t sys_clk_hz, std::uint32_t tick_hz);
    ~Systick();

    void enable() noexcept;
    void disable() noexcept;
    void acknowledge_irq() noexcept;
    std::uint32_t get_interval() const noexcept;
    std::uint32_t get_count() const noexcept;
    std::uint32_t get_overflow() const noexcept;

private:
    struct Registers {
        mmio::Register<32, mmio::ReadWrite> CSR;
        mmio::Register<32, mmio::ReadWrite> RVR;
        mmio::Register<32, mmio::ReadWrite> CVR;
        mmio::Register<32, mmio::ReadWrite> CR;
    };

    std::uint32_t dummy_[4] = {0};
    mmio::address_t base_addr_;
    Registers regs_;
    std::uint32_t reload_value_;
};

} // namespace hal

#endif // SYSTICK_H
