#include "app_main.h"
#include "systick.h"

void app_main() {
    constexpr mmio::address_t SYSTICK_BASE = 0xE000E010U;

    hal::Systick systick(SYSTICK_BASE, 180'000'000U, 1'000U);
    systick.enable();

    volatile auto count = systick.get_count();
    volatile auto overflow = systick.get_overflow();
    (void)count;
    (void)overflow;

    systick.disable();
}
