#include "Mock_Systick.h"
#include <cstdint>

static MockSystickImpl* p_MockInterfaceSystick = nullptr;

MockSystickImpl::MockSystickImpl()
    : dummy_registers_{0, 0, 0, 0}
    , base_address_(reinterpret_cast<mmio::address_t>(&dummy_registers_[0]))
{
    if (p_MockInterfaceSystick == nullptr) {
        p_MockInterfaceSystick = this;
    }
}

MockSystickImpl::~MockSystickImpl() {
    if (p_MockInterfaceSystick == this) {
        p_MockInterfaceSystick = nullptr;
    }
}
