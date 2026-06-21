# MMIO Usage Guide

## Base Address Initialization Modes

The library supports three modes for providing a peripheral's base address to its register block.

### Mode 1: Static (Compile-Time)

Use `StaticRegisterBlock` when the base address is known at compile time:

```cpp
using UartBlock = mmio::StaticRegisterBlock<0x40001000,
    mmio::at_offset<0x00, 32, mmio::ReadWrite>,  // DATA
    mmio::at_offset<0x04, 32, mmio::ReadOnly>,   // STATUS
    mmio::at_offset<0x08, 32, mmio::ReadWrite>   // CTRL
>;

UartBlock uart; // registers initialized at construction
uart.get<2>().set_bit(0, true); // enable
```

### Mode 2: Constructor (Runtime)

Use `DynamicRegisterBlock` with a constructor argument when the base address is determined at runtime:

```cpp
using GpioBlock = mmio::DynamicRegisterBlock<
    mmio::at_offset<0x00, 32, mmio::ReadWrite>,
    mmio::at_offset<0x04, 32, mmio::ReadWrite>
>;

GpioBlock gpio(runtime_base_address);
gpio.get<0>() = 0xFF;
```

Or via `Peripheral`:

```cpp
using GpioPeripheral = mmio::Peripheral<GpioBlock>;

GpioPeripheral gpio(runtime_base_address);
gpio.regs().get<0>() = 0xFF;
```

### Mode 3: Late Initialization

Default-construct, then call `set_base_address()` when ready:

```cpp
GpioBlock gpio;
// ... later, after discovering the address ...
gpio.set_base_address(discovered_address);
gpio.get<0>() = 0xFF;
```

## Writing a Peripheral Driver

```cpp
#include "mmio.hpp"

class Uart {
public:
    explicit Uart(mmio::address_t base) : regs_(base) {}

    void send(std::uint8_t byte) {
        while (regs_.get<1>().get_bit(5)) {} // wait for TX empty
        regs_.get<0>() = byte;
    }

    std::uint8_t receive() {
        while (!regs_.get<1>().get_bit(0)) {} // wait for RX ready
        return static_cast<std::uint8_t>(regs_.get<0>().read());
    }

private:
    using Block = mmio::DynamicRegisterBlock<
        mmio::at_offset<0x00, 32, mmio::ReadWrite>,  // DATA
        mmio::at_offset<0x04, 32, mmio::ReadOnly>,   // STATUS
        mmio::at_offset<0x08, 32, mmio::ReadWrite>   // CTRL
    >;
    Block regs_;
};
```

## Summary

| Mode | Type | When to use |
|------|------|-------------|
| Static | `StaticRegisterBlock<Addr, ...>` | Address known at compile time |
| Constructor | `DynamicRegisterBlock(addr)` | Address known at construction |
| Late | Default + `set_base_address()` | Address discovered after construction |
