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

//-----------------------------------------------------------------------------
/* helpers */

namespace
{

DFSDM_Channel_TypeDef * get_channel_reg(dfsdm::channel::id ch)
{
    return DFSDM_Channel0 + (DFSDM_Channel1 - DFSDM_Channel0) * static_cast<uint8_t>(ch);
}

}

//-----------------------------------------------------------------------------
/* private */

void dfsdm::global_toggle(bool state)
{
    if (state)
        DFSDM_Channel0->CHCFGR1 |= DFSDM_CHCFGR1_DFSDMEN;
    else
        DFSDM_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_DFSDMEN;
}

//-----------------------------------------------------------------------------
/* public */

void dfsdm::enable(void)
{
    rcc::toggle_periph_clock(RCC_PERIPH_BUS(APB2, DFSDM1), true);
    global_toggle(true);
}

void dfsdm::disable(void)
{
    global_toggle(false);
    rcc::toggle_periph_clock(RCC_PERIPH_BUS(APB2, DFSDM1), false);
}

uint32_t dfsdm::configure_clock_output(clk_out_src clk_src, uint32_t frequency, bool enabled)
{
    uint32_t output_clock_freq = 0;

    global_toggle(false);

    if (enabled)
    {
        gpio::init({gpio::port::porte, gpio::pin::pin9}, gpio::mode::af, gpio::af::af6);

        /* Configure clock source */
        uint32_t input_clock_freq = 0;
        switch (clk_src)
        {
            case dfsdm::clk_out_src::apb2:
                DFSDM_Channel0->CHCFGR1 &= ~DFSDM_CHCFGR1_CKOUTSRC;
                input_clock_freq = rcc::get_bus_freq(rcc::bus::APB2);
            break;
            case dfsdm::clk_out_src::sysclk:
                /* TODO: Implement this in RCC driver */
                RCC->CCIPR |= RCC_CCIPR_DFSDM1SEL;
                input_clock_freq = rcc::get_sysclk_freq();
            break;
            case dfsdm::clk_out_src::sai1:
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

    global_toggle(true);
    return output_clock_freq;
}

void dfsdm::channel::configure(id ch, data_pack dp, data_input di, clk_src clk, protocol proto, bool enabled)
{
    global_toggle(false);

    auto channel = get_channel_reg(ch);

    channel->CHCFGR1 &= ~DFSDM_CHCFGR1_DATPACK_Msk;
    channel->CHCFGR1 |= static_cast<uint32_t>(dp) << DFSDM_CHCFGR1_DATPACK_Pos;

    channel->CHCFGR1 &= ~DFSDM_CHCFGR1_DATMPX_Msk;
    channel->CHCFGR1 |= static_cast<uint32_t>(di) << DFSDM_CHCFGR1_DATMPX_Pos;

    channel->CHCFGR1 &= ~DFSDM_CHCFGR1_SPICKSEL_Msk;
    channel->CHCFGR1 |= static_cast<uint32_t>(clk) << DFSDM_CHCFGR1_SPICKSEL_Pos;

    channel->CHCFGR1 &= ~DFSDM_CHCFGR1_SITP_Msk;
    channel->CHCFGR1 |= static_cast<uint32_t>(proto) << DFSDM_CHCFGR1_SITP_Pos;

    if (enabled)
    {
        /* TODO: Make channel-to-pin map */
        gpio::init({gpio::port::porte, gpio::pin::pin7}, gpio::mode::af, gpio::af::af6);
        channel->CHCFGR1 |= DFSDM_CHCFGR1_CHEN;
    }
    else
    {
        gpio::init({gpio::port::porte, gpio::pin::pin7}, gpio::mode::analog);
        channel->CHCFGR1 &= ~DFSDM_CHCFGR1_CHEN;
    }

    global_toggle(true);
}

void dfsdm::channel::set_offset(id ch, uint32_t offset)
{
    auto channel = get_channel_reg(ch);
    channel->CHCFGR2 &= DFSDM_CHCFGR2_OFFSET_Msk;
    channel->CHCFGR2 |= (offset & DFSDM_CHCFGR2_OFFSET_Msk) << DFSDM_CHCFGR2_OFFSET_Pos;
}

void dfsdm::channel::set_bitshift(id ch, uint8_t right_shift)
{
    global_toggle(false);
    auto channel = get_channel_reg(ch);
    channel->CHCFGR2 &= DFSDM_CHCFGR2_DTRBS_Msk;
    channel->CHCFGR2 |= (right_shift & DFSDM_CHCFGR2_DTRBS_Msk) << DFSDM_CHCFGR2_DTRBS_Pos;
    global_toggle(true);
}
