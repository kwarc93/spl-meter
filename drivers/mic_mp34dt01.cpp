/*
 * mic_mp34dt01.cpp
 *
 *  Created on: 22 wrz 2021
 *      Author: kwarc
 */

#include "mic_mp34dt01.hpp"

using namespace drivers;

//-----------------------------------------------------------------------------
/* helpers */

//-----------------------------------------------------------------------------
/* public */

mic_mp34dt01::mic_mp34dt01(dfsdm::channel::id ch, dfsdm::filter::id f) : channel {ch}, filter {f}
{
    this->sampling_frequency = 0;
}

void mic_mp34dt01::init(hal::interface::microphone::data_ready_callback_t data_ready)
{
    /* Digital microphone on channel 2, fclk=2.4MHz, fs=40kHz, 16bit */

    dfsdm::enable(true);
    dfsdm::channel::enable(this->channel, false);
    dfsdm::filter::enable(this->filter, false);

    dfsdm::channel::set_offset(this->channel, 0);
    dfsdm::channel::set_bitshift(this->channel, 1);
    dfsdm::channel::configure(this->channel,
                              dfsdm::channel::data_pack::standard,
                              dfsdm::channel::data_input::ext_pin,
                              dfsdm::channel::clk_src::int_ckout,
                              dfsdm::channel::protocol::spi_r_edge);

    const uint16_t filter_decim = 60;
    const uint8_t integrator_avg = 1;

    dfsdm::filter::configure(this->filter, dfsdm::filter::order::sinc4, filter_decim, integrator_avg);
    dfsdm::filter::link_channel(this->filter, this->channel);
    dfsdm::filter::enable_dma(this->filter, const_cast<int16_t*>(this->samples), sizeof(this->samples) / sizeof(*this->samples) , data_ready);

    uint32_t clk = dfsdm::configure_clock_output(dfsdm::clk_out_src::apb2, 2400000, true);

    this->sampling_frequency = clk / (filter_decim * integrator_avg);
}

void mic_mp34dt01::enable(void)
{
    dfsdm::configure_clock_output(dfsdm::clk_out_src::apb2, 2400000, true);

    dfsdm::channel::enable(this->channel, true);
    dfsdm::filter::enable(this->filter, true);
    dfsdm::filter::trigger(this->filter);
}

void mic_mp34dt01::disable(void)
{
    dfsdm::channel::enable(this->channel, false);
    dfsdm::filter::enable(this->filter, false);

    dfsdm::configure_clock_output(dfsdm::clk_out_src::apb2, 2400000, false);
}


void mic_mp34dt01::set_gain(float gain)
{
    /* TODO */
}

uint32_t mic_mp34dt01::get_snr(void)
{
    return this->snr;
}

uint32_t mic_mp34dt01::get_aop(void)
{
    return this->aop;
}

int32_t mic_mp34dt01::get_sensitivity(void)
{
    return this->sensitivity;
}

uint32_t mic_mp34dt01::get_sampling_frequency(void)
{
    return this->sampling_frequency;
}
