/*
 * interrupt_handlers.cpp
 *
 *  Created on: 21 wrz 2021
 *      Author: kwarc
 */

#include <drivers/stm32l4/dfsdm.hpp>

//-----------------------------------------------------------------------------
/* interrupt handlers */

extern "C" void SysTick_Handler(void)
{

}

extern "C" void DMA1_Channel4_IRQHandler(void)
{
    drivers::dfsdm::dma_handler(drivers::dfsdm::filter::id::f0);
}
