/*
 * interrupt_handlers.cpp
 *
 *  Created on: 21 wrz 2021
 *      Author: kwarc
 */

#include <hal/hal_system.hpp>
#include <drivers/stm32l4/dfsdm.hpp>

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
