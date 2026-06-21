#ifndef MMIO_PERIPHERAL_HPP
#define MMIO_PERIPHERAL_HPP

#include "mmio_register_block.hpp"

namespace mmio {

/// \brief Optional base for peripherals.
/// \tparam RegBlock  Either StaticRegisterBlock or DynamicRegisterBlock.
///
/// This class provides no enable/disable methods – those are left to the
/// derived class, because not all peripherals have clocks/power gates.
template <typename RegBlock>
class Peripheral {
public:
    Peripheral() noexcept = default;

    /// Constructor for dynamic blocks (passes address to the block).
    template <typename T = RegBlock,
              typename = std::enable_if_t<std::is_constructible<T, address_t>::value>>
    explicit Peripheral(address_t base_addr) noexcept : regs_(base_addr) {}

    /// Access the register block.
    RegBlock& regs() noexcept { return regs_; }
    const RegBlock& regs() const noexcept { return regs_; }

protected:
    RegBlock regs_;
};

} // namespace mmio

#endif // MMIO_PERIPHERAL_HPP