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

DFSDM_Filter_TypeDef * get_filter_reg(dfsdm::filter::id f)
{
    return DFSDM_Filter0 + (DFSDM_Filter1 - DFSDM_Filter0) * static_cast<uint8_t>(f);
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

void dfsdm::enable(bool state)
{
    if (state)
    {
        rcc::toggle_periph_clock(RCC_PERIPH_BUS(APB2, DFSDM1), true);
        global_toggle(true);
    }
    else
    {
        global_toggle(false);
        rcc::toggle_periph_clock(RCC_PERIPH_BUS(APB2, DFSDM1), false);
    }
}

uint32_t dfsdm::configure_clock_output(clk_out_src clk_src, uint32_t frequency, bool enabled)
{
    uint32_t output_clock_freq = 0;

    global_toggle(false);

    if (enabled)
    {
        gpio::configure({gpio::port::porte, gpio::pin::pin9}, gpio::mode::af, gpio::af::af6);

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
        if (divider > 0 && divider <= 256)
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

void dfsdm::channel::configure(id ch, data_pack dp, data_input di, clk_src clk, protocol proto)
{
    auto channel = get_channel_reg(ch);

    channel->CHCFGR1 &= ~DFSDM_CHCFGR1_CHEN;

    channel->CHCFGR1 &= ~DFSDM_CHCFGR1_DATPACK_Msk;
    channel->CHCFGR1 |= static_cast<uint32_t>(dp) << DFSDM_CHCFGR1_DATPACK_Pos;

    channel->CHCFGR1 &= ~DFSDM_CHCFGR1_DATMPX_Msk;
    channel->CHCFGR1 |= static_cast<uint32_t>(di) << DFSDM_CHCFGR1_DATMPX_Pos;

    channel->CHCFGR1 &= ~DFSDM_CHCFGR1_SPICKSEL_Msk;
    channel->CHCFGR1 |= static_cast<uint32_t>(clk) << DFSDM_CHCFGR1_SPICKSEL_Pos;

    channel->CHCFGR1 &= ~DFSDM_CHCFGR1_SITP_Msk;
    channel->CHCFGR1 |= static_cast<uint32_t>(proto) << DFSDM_CHCFGR1_SITP_Pos;
}

void dfsdm::channel::set_offset(id ch, uint32_t offset)
{
    auto channel = get_channel_reg(ch);
    channel->CHCFGR2 &= DFSDM_CHCFGR2_OFFSET_Msk;
    channel->CHCFGR2 |= (offset & DFSDM_CHCFGR2_OFFSET_Msk) << DFSDM_CHCFGR2_OFFSET_Pos;
}

void dfsdm::channel::set_bitshift(id ch, uint8_t right_shift)
{
    auto channel = get_channel_reg(ch);
    channel->CHCFGR2 &= DFSDM_CHCFGR2_DTRBS_Msk;
    channel->CHCFGR2 |= (right_shift & DFSDM_CHCFGR2_DTRBS_Msk) << DFSDM_CHCFGR2_DTRBS_Pos;
}

void dfsdm::channel::enable(id ch, bool state)
{
    auto channel = get_channel_reg(ch);

    if (state)
    {
        /* TODO: Make channel-to-pin map (now, only ch2 is supported) */
        gpio::configure({gpio::port::porte, gpio::pin::pin7}, gpio::mode::af, gpio::af::af6);
        channel->CHCFGR1 |= DFSDM_CHCFGR1_CHEN;
    }
    else
    {
        gpio::configure({gpio::port::porte, gpio::pin::pin7}, gpio::mode::analog);
        channel->CHCFGR1 &= ~DFSDM_CHCFGR1_CHEN;
    }
}

void dfsdm::filter::enable(id f, bool state)
{
    auto filter = get_filter_reg(f);

    if (state)
        filter->FLTCR1 |= DFSDM_FLTCR1_DFEN;
    else
        filter->FLTCR1 &= ~DFSDM_FLTCR1_DFEN;
}

void dfsdm::filter::configure(id f, order ord, uint16_t decim, uint8_t avg)
{
    auto filter = get_filter_reg(f);

    filter->FLTFCR &= ~DFSDM_FLTFCR_FORD_Msk;
    filter->FLTFCR |= static_cast<uint32_t>(ord) << DFSDM_FLTFCR_FORD_Pos;

    if (decim > 1023)
        decim = 1023;

    filter->FLTFCR &= ~DFSDM_FLTFCR_FOSR_Msk;
    filter->FLTFCR |= decim << DFSDM_FLTFCR_FOSR_Pos;

    filter->FLTFCR &= ~DFSDM_FLTFCR_IOSR_Msk;
    filter->FLTFCR |= avg << DFSDM_FLTFCR_IOSR_Pos;
}

void dfsdm::filter::configure_conversions(id f, bool fast_mode, bool continous_mode, bool sync_with_f0)
{
    auto filter = get_filter_reg(f);

    if (fast_mode)
        filter->FLTCR1 |= DFSDM_FLTCR1_FAST;
    else
        filter->FLTCR1 &= ~DFSDM_FLTCR1_FAST;

    if (continous_mode)
        filter->FLTCR1 |= DFSDM_FLTCR1_RCONT;
    else
        filter->FLTCR1 &= ~DFSDM_FLTCR1_RCONT;

    if (sync_with_f0)
        filter->FLTCR1 |= DFSDM_FLTCR1_RSYNC;
    else
        filter->FLTCR1 &= ~DFSDM_FLTCR1_RSYNC;
}

void dfsdm::filter::link_channel(id f, channel::id ch)
{
    auto filter = get_filter_reg(f);

    filter->FLTCR1 &= ~DFSDM_FLTCR1_RCH_Msk;
    filter->FLTCR1 |= static_cast<uint32_t>(ch) << DFSDM_FLTCR1_RCH_Pos;
}

void dfsdm::filter::trigger(id f)
{
    auto filter = get_filter_reg(f);
    filter->FLTCR1 |= DFSDM_FLTCR1_RSWSTART;
}

int32_t dfsdm::filter::poll(id f)
{
    auto filter = get_filter_reg(f);

    while(!(filter->FLTISR & DFSDM_FLTISR_REOCF));

    /* Extend int24_t to int32_t with sign */
    struct int24_t { int32_t value:24; };
    return int24_t{static_cast<int32_t>(filter->FLTRDATAR >> 8)}.value;
}
