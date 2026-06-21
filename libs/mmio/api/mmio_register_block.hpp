#ifndef MMIO_REGISTER_BLOCK_HPP
#define MMIO_REGISTER_BLOCK_HPP

#include "mmio_register.hpp"
#include <tuple>
#include <utility>

namespace mmio {

namespace detail {

template <address_t Offset, std::size_t Bits, typename Access>
struct OffsetRegister {
    using reg_type = Register<Bits, Access>;
    static constexpr address_t offset = Offset;
};

} // namespace detail

template <address_t Offset, std::size_t Bits, typename Access = ReadWrite>
using at_offset = detail::OffsetRegister<Offset, Bits, Access>;

// ----------------------------------------------------------------------
// 1. STATIC BLOCK – compile‑time base address
// ----------------------------------------------------------------------
template <address_t BaseAddr, typename... Regs>
class StaticRegisterBlock {
public:
    StaticRegisterBlock() noexcept {
        init(std::index_sequence_for<Regs...>{});
    }

    template <std::size_t I>
    auto& get() noexcept { return std::get<I>(regs_); }

    template <std::size_t I>
    const auto& get() const noexcept { return std::get<I>(regs_); }

private:
    std::tuple<typename Regs::reg_type...> regs_;

    template <std::size_t... Is>
    void init(std::index_sequence<Is...>) noexcept {
        ((std::get<Is>(regs_).set_address(BaseAddr + Regs::offset)), ...);
    }
};

// ----------------------------------------------------------------------
// 2. DYNAMIC BLOCK – runtime base address (constructor or late set)
// ----------------------------------------------------------------------
template <typename... Regs>
class DynamicRegisterBlock {
public:
    explicit DynamicRegisterBlock(address_t base_addr) noexcept {
        set_base_address(base_addr);
    }

    DynamicRegisterBlock() noexcept = default;

    void set_base_address(address_t base_addr) noexcept {
        apply_base(base_addr, std::index_sequence_for<Regs...>{});
    }

    bool is_valid() const noexcept {
        return std::get<0>(regs_).is_valid();
    }

    template <std::size_t I>
    auto& get() noexcept { return std::get<I>(regs_); }

    template <std::size_t I>
    const auto& get() const noexcept { return std::get<I>(regs_); }

private:
    std::tuple<typename Regs::reg_type...> regs_;

    template <std::size_t... Is>
    void apply_base(address_t base, std::index_sequence<Is...>) noexcept {
        ((std::get<Is>(regs_).set_address(base + Regs::offset)), ...);
    }
};

} // namespace mmio

#endif // MMIO_REGISTER_BLOCK_HPP
