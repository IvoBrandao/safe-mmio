#include <cstdint>
#include <algorithm>

extern std::uint32_t _sidata;
extern std::uint32_t _sdata;
extern std::uint32_t _edata;
extern std::uint32_t _sbss;
extern std::uint32_t _ebss;
extern std::uint32_t _estack;

extern "C" void __libc_init_array();
extern "C" int main();

extern "C" void Reset_Handler() {
    std::uint32_t* src = &_sidata;
    std::uint32_t* dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    __libc_init_array();
    main();

    while (true) {}
}

extern "C" {

__attribute__((weak)) void NMI_Handler()                { while (true) {} }
__attribute__((weak)) void HardFault_Handler()          { while (true) {} }
__attribute__((weak)) void MemManage_Handler()          { while (true) {} }
__attribute__((weak)) void BusFault_Handler()           { while (true) {} }
__attribute__((weak)) void UsageFault_Handler()         { while (true) {} }
__attribute__((weak)) void SVC_Handler()                { while (true) {} }
__attribute__((weak)) void DebugMon_Handler()           { while (true) {} }
__attribute__((weak)) void PendSV_Handler()             { while (true) {} }
__attribute__((weak)) void SysTick_Handler()            { while (true) {} }

__attribute__((weak)) void WWDG_IRQHandler()            { while (true) {} }
__attribute__((weak)) void PVD_IRQHandler()             { while (true) {} }
__attribute__((weak)) void TAMP_STAMP_IRQHandler()      { while (true) {} }
__attribute__((weak)) void RTC_WKUP_IRQHandler()        { while (true) {} }
__attribute__((weak)) void FLASH_IRQHandler()           { while (true) {} }
__attribute__((weak)) void RCC_IRQHandler()             { while (true) {} }
__attribute__((weak)) void EXTI0_IRQHandler()           { while (true) {} }
__attribute__((weak)) void EXTI1_IRQHandler()           { while (true) {} }
__attribute__((weak)) void EXTI2_IRQHandler()           { while (true) {} }
__attribute__((weak)) void EXTI3_IRQHandler()           { while (true) {} }
__attribute__((weak)) void EXTI4_IRQHandler()           { while (true) {} }
__attribute__((weak)) void DMA1_Stream0_IRQHandler()    { while (true) {} }
__attribute__((weak)) void DMA1_Stream1_IRQHandler()    { while (true) {} }
__attribute__((weak)) void DMA1_Stream2_IRQHandler()    { while (true) {} }
__attribute__((weak)) void DMA1_Stream3_IRQHandler()    { while (true) {} }
__attribute__((weak)) void DMA1_Stream4_IRQHandler()    { while (true) {} }
__attribute__((weak)) void DMA1_Stream5_IRQHandler()    { while (true) {} }
__attribute__((weak)) void DMA1_Stream6_IRQHandler()    { while (true) {} }
__attribute__((weak)) void ADC_IRQHandler()             { while (true) {} }
__attribute__((weak)) void CAN1_TX_IRQHandler()         { while (true) {} }
__attribute__((weak)) void CAN1_RX0_IRQHandler()        { while (true) {} }
__attribute__((weak)) void CAN1_RX1_IRQHandler()        { while (true) {} }
__attribute__((weak)) void CAN1_SCE_IRQHandler()        { while (true) {} }
__attribute__((weak)) void EXTI9_5_IRQHandler()         { while (true) {} }
__attribute__((weak)) void TIM1_BRK_TIM9_IRQHandler()   { while (true) {} }
__attribute__((weak)) void TIM1_UP_TIM10_IRQHandler()   { while (true) {} }
__attribute__((weak)) void TIM1_TRG_COM_TIM11_IRQHandler() { while (true) {} }
__attribute__((weak)) void TIM1_CC_IRQHandler()         { while (true) {} }
__attribute__((weak)) void TIM2_IRQHandler()            { while (true) {} }
__attribute__((weak)) void TIM3_IRQHandler()            { while (true) {} }
__attribute__((weak)) void TIM4_IRQHandler()            { while (true) {} }
__attribute__((weak)) void I2C1_EV_IRQHandler()         { while (true) {} }
__attribute__((weak)) void I2C1_ER_IRQHandler()         { while (true) {} }
__attribute__((weak)) void I2C2_EV_IRQHandler()         { while (true) {} }
__attribute__((weak)) void I2C2_ER_IRQHandler()         { while (true) {} }
__attribute__((weak)) void SPI1_IRQHandler()            { while (true) {} }
__attribute__((weak)) void SPI2_IRQHandler()            { while (true) {} }
__attribute__((weak)) void USART1_IRQHandler()          { while (true) {} }
__attribute__((weak)) void USART2_IRQHandler()          { while (true) {} }
__attribute__((weak)) void USART3_IRQHandler()          { while (true) {} }
__attribute__((weak)) void EXTI15_10_IRQHandler()       { while (true) {} }
__attribute__((weak)) void RTC_Alarm_IRQHandler()       { while (true) {} }
__attribute__((weak)) void OTG_FS_WKUP_IRQHandler()    { while (true) {} }
__attribute__((weak)) void TIM8_BRK_TIM12_IRQHandler()  { while (true) {} }
__attribute__((weak)) void TIM8_UP_TIM13_IRQHandler()   { while (true) {} }
__attribute__((weak)) void TIM8_TRG_COM_TIM14_IRQHandler() { while (true) {} }
__attribute__((weak)) void TIM8_CC_IRQHandler()         { while (true) {} }
__attribute__((weak)) void DMA1_Stream7_IRQHandler()    { while (true) {} }
__attribute__((weak)) void FMC_IRQHandler()             { while (true) {} }
__attribute__((weak)) void SDIO_IRQHandler()            { while (true) {} }
__attribute__((weak)) void TIM5_IRQHandler()            { while (true) {} }
__attribute__((weak)) void SPI3_IRQHandler()            { while (true) {} }
__attribute__((weak)) void UART4_IRQHandler()           { while (true) {} }
__attribute__((weak)) void UART5_IRQHandler()           { while (true) {} }
__attribute__((weak)) void TIM6_DAC_IRQHandler()        { while (true) {} }
__attribute__((weak)) void TIM7_IRQHandler()            { while (true) {} }
__attribute__((weak)) void DMA2_Stream0_IRQHandler()    { while (true) {} }
__attribute__((weak)) void DMA2_Stream1_IRQHandler()    { while (true) {} }
__attribute__((weak)) void DMA2_Stream2_IRQHandler()    { while (true) {} }
__attribute__((weak)) void DMA2_Stream3_IRQHandler()    { while (true) {} }
__attribute__((weak)) void DMA2_Stream4_IRQHandler()    { while (true) {} }
__attribute__((weak)) void ETH_IRQHandler()             { while (true) {} }
__attribute__((weak)) void ETH_WKUP_IRQHandler()        { while (true) {} }
__attribute__((weak)) void CAN2_TX_IRQHandler()         { while (true) {} }
__attribute__((weak)) void CAN2_RX0_IRQHandler()        { while (true) {} }
__attribute__((weak)) void CAN2_RX1_IRQHandler()        { while (true) {} }
__attribute__((weak)) void CAN2_SCE_IRQHandler()        { while (true) {} }
__attribute__((weak)) void OTG_FS_IRQHandler()          { while (true) {} }
__attribute__((weak)) void DMA2_Stream5_IRQHandler()    { while (true) {} }
__attribute__((weak)) void DMA2_Stream6_IRQHandler()    { while (true) {} }
__attribute__((weak)) void DMA2_Stream7_IRQHandler()    { while (true) {} }
__attribute__((weak)) void USART6_IRQHandler()          { while (true) {} }
__attribute__((weak)) void I2C3_EV_IRQHandler()         { while (true) {} }
__attribute__((weak)) void I2C3_ER_IRQHandler()         { while (true) {} }
__attribute__((weak)) void OTG_HS_EP1_OUT_IRQHandler()  { while (true) {} }
__attribute__((weak)) void OTG_HS_EP1_IN_IRQHandler()   { while (true) {} }
__attribute__((weak)) void OTG_HS_WKUP_IRQHandler()     { while (true) {} }
__attribute__((weak)) void OTG_HS_IRQHandler()          { while (true) {} }
__attribute__((weak)) void DCMI_IRQHandler()            { while (true) {} }
__attribute__((weak)) void HASH_RNG_IRQHandler()        { while (true) {} }
__attribute__((weak)) void FPU_IRQHandler()             { while (true) {} }
__attribute__((weak)) void UART7_IRQHandler()           { while (true) {} }
__attribute__((weak)) void UART8_IRQHandler()           { while (true) {} }
__attribute__((weak)) void SPI4_IRQHandler()            { while (true) {} }
__attribute__((weak)) void SPI5_IRQHandler()            { while (true) {} }
__attribute__((weak)) void SPI6_IRQHandler()            { while (true) {} }
__attribute__((weak)) void SAI1_IRQHandler()            { while (true) {} }
__attribute__((weak)) void LTDC_IRQHandler()            { while (true) {} }
__attribute__((weak)) void LTDC_ER_IRQHandler()         { while (true) {} }
__attribute__((weak)) void DMA2D_IRQHandler()           { while (true) {} }

} // extern "C"

using isr_t = void(*)();

__attribute__((section(".isr_vector"), used))
const isr_t vector_table[] = {
    reinterpret_cast<isr_t>(&_estack),
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    SVC_Handler,
    DebugMon_Handler,
    nullptr,
    PendSV_Handler,
    SysTick_Handler,
    WWDG_IRQHandler,
    PVD_IRQHandler,
    TAMP_STAMP_IRQHandler,
    RTC_WKUP_IRQHandler,
    FLASH_IRQHandler,
    RCC_IRQHandler,
    EXTI0_IRQHandler,
    EXTI1_IRQHandler,
    EXTI2_IRQHandler,
    EXTI3_IRQHandler,
    EXTI4_IRQHandler,
    DMA1_Stream0_IRQHandler,
    DMA1_Stream1_IRQHandler,
    DMA1_Stream2_IRQHandler,
    DMA1_Stream3_IRQHandler,
    DMA1_Stream4_IRQHandler,
    DMA1_Stream5_IRQHandler,
    DMA1_Stream6_IRQHandler,
    ADC_IRQHandler,
    CAN1_TX_IRQHandler,
    CAN1_RX0_IRQHandler,
    CAN1_RX1_IRQHandler,
    CAN1_SCE_IRQHandler,
    EXTI9_5_IRQHandler,
    TIM1_BRK_TIM9_IRQHandler,
    TIM1_UP_TIM10_IRQHandler,
    TIM1_TRG_COM_TIM11_IRQHandler,
    TIM1_CC_IRQHandler,
    TIM2_IRQHandler,
    TIM3_IRQHandler,
    TIM4_IRQHandler,
    I2C1_EV_IRQHandler,
    I2C1_ER_IRQHandler,
    I2C2_EV_IRQHandler,
    I2C2_ER_IRQHandler,
    SPI1_IRQHandler,
    SPI2_IRQHandler,
    USART1_IRQHandler,
    USART2_IRQHandler,
    USART3_IRQHandler,
    EXTI15_10_IRQHandler,
    RTC_Alarm_IRQHandler,
    OTG_FS_WKUP_IRQHandler,
    TIM8_BRK_TIM12_IRQHandler,
    TIM8_UP_TIM13_IRQHandler,
    TIM8_TRG_COM_TIM14_IRQHandler,
    TIM8_CC_IRQHandler,
    DMA1_Stream7_IRQHandler,
    FMC_IRQHandler,
    SDIO_IRQHandler,
    TIM5_IRQHandler,
    SPI3_IRQHandler,
    UART4_IRQHandler,
    UART5_IRQHandler,
    TIM6_DAC_IRQHandler,
    TIM7_IRQHandler,
    DMA2_Stream0_IRQHandler,
    DMA2_Stream1_IRQHandler,
    DMA2_Stream2_IRQHandler,
    DMA2_Stream3_IRQHandler,
    DMA2_Stream4_IRQHandler,
    ETH_IRQHandler,
    ETH_WKUP_IRQHandler,
    CAN2_TX_IRQHandler,
    CAN2_RX0_IRQHandler,
    CAN2_RX1_IRQHandler,
    CAN2_SCE_IRQHandler,
    OTG_FS_IRQHandler,
    DMA2_Stream5_IRQHandler,
    DMA2_Stream6_IRQHandler,
    DMA2_Stream7_IRQHandler,
    USART6_IRQHandler,
    I2C3_EV_IRQHandler,
    I2C3_ER_IRQHandler,
    OTG_HS_EP1_OUT_IRQHandler,
    OTG_HS_EP1_IN_IRQHandler,
    OTG_HS_WKUP_IRQHandler,
    OTG_HS_IRQHandler,
    DCMI_IRQHandler,
    nullptr,
    HASH_RNG_IRQHandler,
    FPU_IRQHandler,
    UART7_IRQHandler,
    UART8_IRQHandler,
    SPI4_IRQHandler,
    SPI5_IRQHandler,
    SPI6_IRQHandler,
    SAI1_IRQHandler,
    LTDC_IRQHandler,
    LTDC_ER_IRQHandler,
    DMA2D_IRQHandler,
};
