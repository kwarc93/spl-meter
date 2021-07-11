/*
 * dfsdm.cpp
 *
 *  Created on: 11 lip 2021
 *      Author: kwarc
 */

#include "dfsdm.hpp"

#include <cmsis/stm32l4xx.h>

#include <drivers/stm32l4/rcc.hpp>
#include <drivers/stm32l4/gpio.hpp>

using namespace drivers;

void dfsdm::enable(void)
{
    rcc::enable_periph_clock(RCC_PERIPH_BUS(APB2, DFSDM1), true);
    DFSDM_Channel0->CHCFGR1 |= DFSDM_CHCFGR1_DFSDMEN;
}

void dfsdm::disable(void)
{
    DFSDM_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_DFSDMEN;
    rcc::enable_periph_clock(RCC_PERIPH_BUS(APB2, DFSDM1), false);
}

uint32_t dfsdm::configure_clock_output(bool enabled, clk_out_src clk_src, uint32_t frequency)
{
    uint32_t output_clock_freq = 0;

    DFSDM_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_DFSDMEN;

    if (enabled)
    {
        gpio::init({gpio::port::porte, gpio::pin::pin9}, gpio::af::af6, gpio::mode::af);

        /* Configure clock source */
        uint32_t input_clock_freq = 0;
        switch (clk_src)
        {
            case dfsdm::clk_out_src::APB2_CLK:
                DFSDM_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_CKOUTSRC;
                input_clock_freq = rcc::get_bus_freq(rcc::bus::APB2);
            break;
            case dfsdm::clk_out_src::SYS_CLK:
                /* TODO: Implement this in RCC driver */
                RCC->CCIPR |= RCC_CCIPR_DFSDM1SEL;
                input_clock_freq = rcc::get_sysclk_freq();
            break;
            case dfsdm::clk_out_src::SAI1_CLK:
                DFSDM_Channel0->CHCFGR1 |= DFSDM_CHCFGR1_CKOUTSRC;
                /* TODO: Get SAI1 clock */
                input_clock_freq = 0;
            break;
            default:
                input_clock_freq = 0;
        }

        /* Configure divider */
        DFSDM_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_CKOUTDIV_Msk;
        uint8_t divider = (input_clock_freq + frequency / 2) / frequency;
        if (divider >= 2 && divider <= 256)
        {
            DFSDM_Channel0->CHCFGR1 |= (divider - 1) << DFSDM_CHCFGR1_CKOUTDIV_Pos;
            output_clock_freq = input_clock_freq / divider;
        }
    }
    else
    {
        DFSDM_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_CKOUTDIV_Msk;
    }

    DFSDM_Channel0->CHCFGR1 |= DFSDM_CHCFGR1_DFSDMEN;
    return output_clock_freq;
}
