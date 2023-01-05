/*
 * interrupt_handlers.cpp
 *
 *  Created on: 21 wrz 2021
 *      Author: kwarc
 */

#include <hal/hal_system.hpp>

#include <drivers/stm32l4/dfsdm.hpp>
#include <drivers/stm32l4/usart.hpp>

//-----------------------------------------------------------------------------
/* interrupt handlers */

extern "C" void SysTick_Handler(void)
{
    hal::system::systick++;
}

extern "C" void DMA1_Channel4_IRQHandler(void)
{
    drivers::dfsdm::dma_irq_handler(drivers::dfsdm::filter::id::f0);
}

extern "C" void USART2_IRQHandler(void)
{
    drivers::usart::instance[static_cast<uint8_t>(drivers::usart::id::usart2)]->irq_handler();
}

