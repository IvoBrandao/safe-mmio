#ifndef MMIO_CORE_HPP
#define MMIO_CORE_HPP

#include <cstdint>
#include <type_traits>

namespace mmio {

/// Type used for memory addresses.
using address_t = std::uintptr_t;

/// Register size traits – maps bit width to underlying unsigned integer type.
template <unsigned int Bits>
struct RegisterTraits {
    static_assert(Bits == 8 || Bits == 16 || Bits == 32 || Bits == 64,
                  "Only 8, 16, 32, 64 bit registers are supported");
    using Type = void;
};

template <> struct RegisterTraits<8>  { using Type = std::uint8_t;  };
template <> struct RegisterTraits<16> { using Type = std::uint16_t; };
template <> struct RegisterTraits<32> { using Type = std::uint32_t; };
template <> struct RegisterTraits<64> { using Type = std::uint64_t; };

/// Access policy tags.
struct ReadOnly  {};
struct WriteOnly {};
struct ReadWrite : ReadOnly, WriteOnly {};

/// Compile‑time debug enable (set to true to enable output).
constexpr bool enable_debug = false;

/// Debug output function – user can override by providing their own.
/// Default implementation uses a no‑op if debug is disabled.
template <typename T>
inline void debug_print([[maybe_unused]] const char* message,
                        [[maybe_unused]] address_t addr,
                        [[maybe_unused]] T value) {
    if constexpr (enable_debug) {
        (void)message;
        (void)addr;
        (void)value;
    }
}

} // namespace mmio

#endif // MMIO_CORE_HPP
