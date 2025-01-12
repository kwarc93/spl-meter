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

DFSDM_Channel_TypeDef *get_channel_reg(dfsdm::channel::id ch)
{
    return DFSDM_Channel0 + (DFSDM_Channel1 - DFSDM_Channel0) * static_cast<uint8_t>(ch);
}

DFSDM_Filter_TypeDef *get_filter_reg(dfsdm::filter::id f)
{
    return DFSDM_Filter0 + (DFSDM_Filter1 - DFSDM_Filter0) * static_cast<uint8_t>(f);
}

DMA_Channel_TypeDef *get_dma_channel_reg(dfsdm::filter::id f)
{
    return DMA1_Channel4 + (DMA1_Channel5 - DMA1_Channel4) * static_cast<uint8_t>(f);
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


void dfsdm::dma_irq_handler(filter::id f)
{
    if (!(get_filter_reg(f)->FLTCR1 & DFSDM_FLTCR1_RDMAEN))
        return;

    uint8_t filter_id = static_cast<uint8_t>(f);

    /* Half-Transfer */
    if (DMA1->ISR & (1 << (DMA_ISR_HTIF4_Pos + filter_id * 4)))
    {
        DMA1->IFCR |= (1 << (DMA_IFCR_CHTIF4_Pos + filter_id * 4));

        if (output_data[filter_id].ready_callback != nullptr)
            output_data[filter_id].ready_callback(&output_data[filter_id].buffer[0], output_data[filter_id].buffer_len / 2);
    }

    /* Transfer Complete */
    if (DMA1->ISR & (1 << (DMA_ISR_TCIF4_Pos + filter_id * 4)))
    {
        DMA1->IFCR |= (1 << (DMA_IFCR_CTCIF4_Pos + filter_id * 4));

        if (output_data[filter_id].ready_callback != nullptr)
            output_data[filter_id].ready_callback(&output_data[filter_id].buffer[output_data[filter_id].buffer_len / 2], output_data[filter_id].buffer_len / 2);
    }
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

void dfsdm::channel::set_offset(id ch, int32_t offset)
{
    auto channel = get_channel_reg(ch);
    channel->CHCFGR2 &= DFSDM_CHCFGR2_OFFSET_Msk;
    channel->CHCFGR2 |= static_cast<uint32_t>(offset) << DFSDM_CHCFGR2_OFFSET_Pos;
}

void dfsdm::channel::set_bitshift(id ch, uint8_t right_shift)
{
    auto channel = get_channel_reg(ch);
    channel->CHCFGR2 &= DFSDM_CHCFGR2_DTRBS_Msk;
    channel->CHCFGR2 |= ((right_shift) << DFSDM_CHCFGR2_DTRBS_Pos) & DFSDM_CHCFGR2_DTRBS_Msk;
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

void dfsdm::filter::enable_dma(id f, int16_t *data_buffer, uint16_t data_buffer_len, const data_ready_cb_t &data_ready_cb)
{
    const uint8_t filter_id = static_cast<uint8_t>(f);

    /* Save output data buffer properties */
    output_data[filter_id].buffer = data_buffer;
    output_data[filter_id].buffer_len = data_buffer_len;
    output_data[filter_id].ready_callback = data_ready_cb;

    /* Enable filter DMA requests */
    auto filter = get_filter_reg(f);
    filter->FLTCR1 |= DFSDM_FLTCR1_RDMAEN;

    /* Configure DMA */
    auto dma_ch = get_dma_channel_reg(f);

    rcc::toggle_periph_clock(RCC_PERIPH_BUS(AHB1, DMA1), true);
    dma_ch->CCR &= ~DMA_CCR_EN;

    /* Data flow: periph > mem; pDataSize: 16bit; mDataSize: 16bit(increment), circular mode */
    dma_ch->CCR = DMA_CCR_MINC | DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0 | DMA_CCR_CIRC;
    dma_ch->CCR |= DMA_CCR_HTIE | DMA_CCR_TCIE;
    dma_ch->CPAR = reinterpret_cast<uint32_t>(&filter->FLTRDATAR) + sizeof(uint16_t);
    dma_ch->CMAR = reinterpret_cast<uint32_t>(data_buffer);
    dma_ch->CNDTR = data_buffer_len;

    DMA1_CSELR->CSELR &= ~(0b1111 << (12 + filter_id * 4));

    /* Priority level */
    IRQn_Type nvic_irq = static_cast<IRQn_Type>(DMA1_Channel4_IRQn + filter_id);
    NVIC_SetPriority(nvic_irq, NVIC_EncodePriority( NVIC_GetPriorityGrouping(), 0, 0 ));

    /* Enable IRQ */
    NVIC_EnableIRQ(nvic_irq);

    /* Start DMA */
    dma_ch->CCR |= DMA_CCR_EN;
}

void dfsdm::filter::configure(id f, order ord, uint16_t decim, uint8_t avg, bool continous_mode, bool fast_mode, bool sync_with_f0)
{
    auto filter = get_filter_reg(f);

    filter->FLTFCR &= ~DFSDM_FLTFCR_FORD_Msk;
    filter->FLTFCR |= static_cast<uint32_t>(ord) << DFSDM_FLTFCR_FORD_Pos;

    /* Trim value */
    if (decim == 0)
        decim = 1;

    if (decim > 1023)
        decim = 1023;

    filter->FLTFCR &= ~DFSDM_FLTFCR_FOSR_Msk;
    filter->FLTFCR |= (decim - 1) << DFSDM_FLTFCR_FOSR_Pos;

    /* Trim value */
    if (avg == 0)
        avg = 1;

    filter->FLTFCR &= ~DFSDM_FLTFCR_IOSR_Msk;
    filter->FLTFCR |= (avg - 1) << DFSDM_FLTFCR_IOSR_Pos;

    if (continous_mode)
        filter->FLTCR1 |= DFSDM_FLTCR1_RCONT;
    else
        filter->FLTCR1 &= ~DFSDM_FLTCR1_RCONT;

    if (fast_mode)
        filter->FLTCR1 |= DFSDM_FLTCR1_FAST;
    else
        filter->FLTCR1 &= ~DFSDM_FLTCR1_FAST;

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

int32_t dfsdm::filter::read(id f)
{
    auto filter = get_filter_reg(f);

    while(!(filter->FLTISR & DFSDM_FLTISR_REOCF));

    /* Extend int24_t to int32_t with sign */
    struct int24_t { int32_t value:24; };
    return int24_t{static_cast<int32_t>(filter->FLTRDATAR >> 8)}.value;
}
