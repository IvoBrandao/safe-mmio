#include "systick.h"

namespace hal {

namespace {
enum class Control : std::size_t {
    ENABLE    = 0,
    TICKINT   = 1,
    CLKSOURCE = 2,
    COUNTFLAG = 16
};
} // namespace

Systick::Systick(mmio::address_t base, std::uint32_t sys_clk_hz, std::uint32_t tick_hz)
    : base_addr_(base ? base : reinterpret_cast<mmio::address_t>(&dummy_[0]))
{
    regs_.CSR.set_address(base_addr_);
    regs_.RVR.set_address(base_addr_ + 0x04U);
    regs_.CVR.set_address(base_addr_ + 0x08U);
    regs_.CR.set_address(base_addr_ + 0x0CU);

    std::uint32_t divider = sys_clk_hz / tick_hz;
    if (divider == 0) divider = 1;
    reload_value_ = divider - 1U;
    if (reload_value_ > 0x00FFFFFFU) reload_value_ = 0x00FFFFFFU;
    if (reload_value_ < 1U) reload_value_ = 1U;

    regs_.CSR.set_bit(static_cast<std::size_t>(Control::CLKSOURCE), true);
    regs_.CSR.set_bit(static_cast<std::size_t>(Control::TICKINT), true);
    regs_.RVR = reload_value_;
    regs_.CVR = 0U;
}

Systick::~Systick() { disable(); }

void Systick::enable() noexcept {
    regs_.CSR.set_bit(static_cast<std::size_t>(Control::ENABLE), true);
}

void Systick::disable() noexcept {
    regs_.CSR.set_bit(static_cast<std::size_t>(Control::ENABLE), false);
}

void Systick::acknowledge_irq() noexcept {
    (void)regs_.CSR.read();
}

std::uint32_t Systick::get_interval() const noexcept {
    return reload_value_ + 1U;
}

std::uint32_t Systick::get_count() const noexcept {
    return regs_.RVR.read() - regs_.CVR.read();
}

std::uint32_t Systick::get_overflow() const noexcept {
    return (regs_.CSR.read() >> 16U) & 1U;
}

} // namespace hal
