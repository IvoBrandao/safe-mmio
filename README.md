# safe-mmio — Type-Safe Memory-Mapped I/O Library

A zero-overhead, header-only C++17 library that provides type-safe, compile-time enforced access to memory-mapped hardware registers. Designed for embedded C++ drivers, with an example for STM32 and ARM Cortex-M.

## What It Does

In embedded systems, peripherals (UART, SPI, GPIO, timers) are controlled by reading and writing to specific memory addresses. Traditionally this is done with raw pointer casts and `#define` macros — error-prone, untyped, and hard to test.

**safe-mmio** replaces that with a strongly-typed register abstraction:

```cpp
// Traditional approach — no type safety, no access control
#define GPIOA_MODER  (*(volatile uint32_t*)0x48000000)
GPIOA_MODER |= (1 << 10);  // hope you got the address right

// safe-mmio — compile-time enforced, testable
mmio::Register<32, mmio::ReadWrite> moder(0x48000000);
moder.set_bit(10, true);  // type-safe, bounds-checked in debug
```

## Features

| Feature | Description |
|---------|-------------|
| **Register sizes** | 8, 16, 32, 64-bit with matching uint types |
| **Access policies** | `ReadOnly`, `WriteOnly`, `ReadWrite` — enforced at compile time |
| **Bit operations** | `set_bit()`, `get_bit()` for single bits |
| **Field access** | `field(mask, shift)` for multi-bit fields |
| **Operators** | `=`, `|=`,`&=`,`^=`,`+=`,`-=`,`++`,`--` |
| **Register blocks** | Group registers at known offsets from a base address |
| **Three init modes** | Static (compile-time), constructor (runtime), late (two-phase) |
| **Atomic RMW** | `AtomicRegister` wraps operations in interrupt-safe critical sections |
| **Zero cost** | No heap, no RTTI, no exceptions, no virtual calls |
| **Testable** | Registers can point to stack variables for host-side unit testing |

## Quick Start

```cpp
#include "mmio.hpp"
using namespace mmio;

// Single register
Register<32, ReadWrite> ctrl(0x40000000);
ctrl = 0x01;
ctrl.set_bit(3, true);

// Multi-bit field: bits [6:4]
auto mode = ctrl.field(0x7, 4);
mode = 0x5;

// Register block with offsets
using UartBlock = DynamicRegisterBlock<
    at_offset<0x00, 32, ReadWrite>,   // DATA
    at_offset<0x04, 32, ReadOnly>,    // STATUS
    at_offset<0x08, 32, ReadWrite>    // CTRL
>;

UartBlock uart(0x40001000);
uart.get<0>() = 'A';                       // write DATA
bool tx_empty = uart.get<1>().get_bit(7);   // read STATUS bit
```

## Building

### Unit tests (host)

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build
```

### Firmware (STM32F429ZI)

```sh
cmake -S . -B build-arm \
    -DCMAKE_TOOLCHAIN_FILE=tools/cmake/arm-none-eabi-gcc.cmake \
    -DCMAKE_BUILD_TYPE=Release
cmake --build build-arm
# Output: Example_Project.elf, .hex, .bin
```

## Project Structure

```
├── app/                    Application layer
│   ├── api/                Public headers
│   ├── source/             main.cpp, app_main.cpp
│   └── tests/              Integration tests
├── libs/
│   ├── mmio/               Header-only MMIO library
│   │   ├── api/            Public headers (mmio.hpp, mmio_register.hpp, ...)
│   │   ├── docs/           Architecture & usage documentation
│   │   └── tests/          Unit tests (mocks, fakes, test sources)
│   ├── systick/            SysTick peripheral driver (example)
│   │   ├── api/            Public header (systick.h)
│   │   ├── source/         Implementation
│   │   └── tests/          Unit tests (mocks, fakes, test sources)
│   └── startup/            STM32F429ZI startup & linker script
│       ├── source/         Vector table, Reset_Handler
│       └── STM32F429ZITx.ld
└── tools/cmake/            Toolchain files & CMake modules
```

## Documentation

Detailed architecture diagrams and usage guides are in [`libs/mmio/docs/`](libs/mmio/docs/):

- [Architecture](libs/mmio/docs/01-arch.md) — module structure, class diagram, design decisions
- [Usage Guide](libs/mmio/docs/usage.md) — initialization modes, driver patterns, testing

## License

MIT License. See LICENSE for details.
