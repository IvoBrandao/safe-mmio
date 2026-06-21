#include "Mock_Register.hpp"
#include <cstdint>

static MockRegisterImpl* p_MockInterfaceRegister = nullptr;

MockRegisterImpl::MockRegisterImpl()
    : dummy_register_(0)
    , base_address_(reinterpret_cast<mmio::address_t>(&dummy_register_))
{
    if (p_MockInterfaceRegister == nullptr) {
        p_MockInterfaceRegister = this;
    }
}

MockRegisterImpl::~MockRegisterImpl() {
    if (p_MockInterfaceRegister == this) {
        p_MockInterfaceRegister = nullptr;
    }
}
