# MMIO Library — Architecture

## Purpose

`mmio-cpp` provides a type-safe abstraction over raw hardware register access. Instead of casting addresses to `volatile` pointers manually, developers declare registers with their size (8/16/32/64 bits) and access policy (read, write, or both). The compiler enforces correctness — writing to a read-only register is a compile error, not a runtime bug.

## Design Goals

1. **Zero overhead** — every operation compiles to the same instructions as hand-written volatile access
2. **Compile-time safety** — invalid operations fail at build time, not on the target
3. **Testable on host** — registers can be backed by stack variables for unit testing without hardware
4. **No runtime cost** — no heap, no RTTI, no exceptions, no virtual dispatch
5. **Composable** — registers group into blocks, blocks compose into peripherals

## Module Structure

```mermaid
graph TD
    subgraph "Public API"
        MMIO[mmio.hpp]
    end

    subgraph "Core Components"
        CORE[mmio_core.hpp<br/>address_t, RegisterTraits, access tags]
        REG[mmio_register.hpp<br/>Register, FieldProxy]
        GUARD[mmio_irq_guard.hpp<br/>IrqGuard RAII]
        ATOMIC[mmio_atomic.hpp<br/>AtomicRegister]
        BLOCK[mmio_register_block.hpp<br/>Static & DynamicRegisterBlock]
        PERIPH[mmio_peripheral.hpp<br/>Peripheral base]
    end

    MMIO --> REG
    MMIO --> GUARD
    MMIO --> ATOMIC
    MMIO --> BLOCK
    MMIO --> PERIPH

    REG --> CORE
    ATOMIC --> REG
    ATOMIC --> GUARD
    BLOCK --> REG
    PERIPH --> BLOCK
```

## Class Diagram

```mermaid
classDiagram
    class Register~Bits, Access~ {
        -volatile BaseType* raw_ptr_
        +Register()
        +Register(address_t)
        +read() BaseType
        +write(BaseType)
        +get_bit(size_t) bool
        +set_bit(size_t, bool)
        +field(mask, shift) FieldProxy
        +get_address() address_t
        +set_address(address_t)
        +is_valid() bool
        +operator=(BaseType)
        +operator BaseType()
        +operator|=(BaseType)
        +operator&=(BaseType)
        +operator^=(BaseType)
        +operator+=(BaseType)
        +operator-=(BaseType)
        +operator++()
        +operator--()
    }

    class FieldProxy {
        -Register& reg_
        -BaseType mask_
        -size_t shift_
        +operator BaseType()
        +operator=(BaseType)
    }

    class AtomicRegister~Bits, Access~ {
        -Register& reg_
        +set_bits(mask)
        +clear_bits(mask)
        +toggle_bits(mask)
        +modify(Func)
    }

    class IrqGuard {
        -uint32_t primask_
        +IrqGuard() «disable IRQ»
        +~IrqGuard() «restore IRQ»
    }

    class StaticRegisterBlock~BaseAddr, Regs...~ {
        -tuple~reg_types...~ regs_
        +get~I~() Register&
    }

    class DynamicRegisterBlock~Regs...~ {
        -tuple~reg_types...~ regs_
        +DynamicRegisterBlock()
        +DynamicRegisterBlock(address_t)
        +set_base_address(address_t)
        +is_valid() bool
        +get~I~() Register&
    }

    class Peripheral~RegBlock~ {
        #RegBlock regs_
        +Peripheral()
        +Peripheral(address_t)
        +regs() RegBlock&
    }

    Register *-- FieldProxy : contains
    AtomicRegister --> Register : wraps
    AtomicRegister ..> IrqGuard : uses
    StaticRegisterBlock o-- Register : owns N
    DynamicRegisterBlock o-- Register : owns N
    Peripheral o-- DynamicRegisterBlock
    Peripheral o-- StaticRegisterBlock
```

## Access Policy Enforcement

```mermaid
graph LR
    subgraph "Access Tags (compile-time)"
        RO[ReadOnly]
        WO[WriteOnly]
        RW[ReadWrite]
    end

    RW -->|inherits| RO
    RW -->|inherits| WO

    subgraph "SFINAE gates"
        R["read / get_bit / operator T"]
        W["write / set_bit / operator= / compound assign"]
    end

    RO -.->|enables| R
    WO -.->|enables| W
    RW -.->|enables| R
    RW -.->|enables| W
```

Attempting to call `write()` on a `Register<32, ReadOnly>` triggers a substitution failure — the code does not compile. No runtime check needed.

## Register Block Initialization Modes

```mermaid
stateDiagram-v2
    [*] --> Static: Base address known at compile time
    [*] --> Constructor: Base address known at runtime
    [*] --> Late: Base address discovered later

    Static --> Ready: StaticRegisterBlock constructor
    Constructor --> Ready: DynamicRegisterBlock(addr)
    Late --> Uninitialized: Default constructor
    Uninitialized --> Ready: set_base_address(addr)

    Ready --> Operating: read() / write() / operators
```

## Atomic Operations Flow

```mermaid
sequenceDiagram
    participant Driver
    participant AtomicRegister
    participant IrqGuard
    participant Register
    participant HW as Hardware Register

    Driver->>AtomicRegister: set_bits(mask)
    AtomicRegister->>IrqGuard: construct (CPSID i)
    Note right of IrqGuard: Interrupts disabled
    AtomicRegister->>Register: read()
    Register->>HW: volatile load
    HW-->>Register: value
    Register-->>AtomicRegister: value
    AtomicRegister->>AtomicRegister: value |= mask
    AtomicRegister->>Register: write(value)
    Register->>HW: volatile store
    AtomicRegister->>IrqGuard: destruct (MSR PRIMASK)
    Note right of IrqGuard: Interrupts restored
```

## Memory Layout (no heap)

The entire library operates on the stack or in static storage:

```
┌─────────────────────────────────────┐
│ Register                            │
│   raw_ptr_: volatile T* (1 pointer) │
│   sizeof = sizeof(void*)            │
├─────────────────────────────────────┤
│ DynamicRegisterBlock<R0, R1, R2>    │
│   tuple<Register, Register, Register>│
│   sizeof = N × sizeof(void*)        │
├─────────────────────────────────────┤
│ Peripheral<Block>                   │
│   block_: Block                     │
│   sizeof = sizeof(Block)            │
└─────────────────────────────────────┘

Heap: never used
Virtual table: none
```

## Design Decisions

| Decision | Rationale |
|----------|-----------|
| Header-only | No link-time surprises; enables LTO to inline everything |
| No virtual methods | Zero overhead — peripheral drivers inherit statically |
| `volatile` via pointer, not member | Allows default-constructed "null" registers for testing |
| SFINAE over concepts | C++17 compatibility (concepts require C++20) |
| Fold expressions for init | Clean O(1) expansion for any number of registers |
| Separate `IrqGuard` | Platform-specific; easily replaced per MCU family |
| `is_valid()` check | Supports two-phase init without undefined behavior |
