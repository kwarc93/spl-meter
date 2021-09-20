/*
 * main.c
 *
 *  Created on: 1 cze 2021
 *      Author: kwarc
 */

#include <iostream>
#include <string>

#include <hal/hal_system.hpp>
#include <hal/hal_delay.hpp>
#include <hal/hal_led.hpp>

#include <drivers/lcd_gh08172.hpp>
#include <drivers/stm32l4/dfsdm.hpp>

#include <cmsis/arm_math.h>

static const unsigned buffer_length = 4096;
static volatile int16_t samples_buffer[buffer_length];
static volatile float32_t dsp_samples_buffer[buffer_length / 2];
static volatile float32_t rms = 0;

void dfsdm_data_ready(const int16_t *data, uint16_t data_len)
{
    /* Copy integer values to DSP buffer */
    for (uint32_t i = 0; i < data_len; i++)
        dsp_samples_buffer[i] = data[i];

    /* Delete offset */
    float32_t *dsp_buffer = const_cast<float32_t*>(dsp_samples_buffer);
    float32_t mean;
    arm_mean_f32(dsp_buffer, data_len, &mean);
    arm_offset_f32(dsp_buffer, (-1.0f)*mean, const_cast<float32_t*>(dsp_samples_buffer), data_len);

    /* Calculate RMS value */
    arm_rms_f32(dsp_buffer, data_len, const_cast<float32_t*>(&rms));
}

int main(void)
{
    hal::system::init();

    std::cout << "System started" << std::endl;

    auto lcd = new drivers::lcd_gh08172();
    auto debug_led = new hal::leds::debug();

    /* LCD test */
    uint8_t spl_value = 95;
    uint8_t padding = spl_value / 100 ? 0 : spl_value / 10 ? 1 : 2;
    std::string lcd_str = "A:" + std::string(padding, ' ') + std::to_string(spl_value) + "dB";
    lcd->write(lcd_str);

    /* DFSDM test (Digital microphone on channel 2, fclk=2.4MHz, fs=18.75kHz, 16bit*/
    using drivers::dfsdm;
    auto dfsdm_channel = dfsdm::channel::id::ch2;
    auto dfsdm_filter = dfsdm::filter::id::f0;

    dfsdm::enable(true);
    dfsdm::channel::enable(dfsdm_channel, false);
    dfsdm::filter::enable(dfsdm_filter, false);

    dfsdm::channel::set_offset(dfsdm_channel, 0);
    dfsdm::channel::set_bitshift(dfsdm_channel, 2);
    dfsdm::channel::configure(dfsdm_channel,
                              dfsdm::channel::data_pack::standard,
                              dfsdm::channel::data_input::ext_pin,
                              dfsdm::channel::clk_src::int_ckout,
                              dfsdm::channel::protocol::spi_r_edge);

    dfsdm::filter::configure(dfsdm_filter, dfsdm::filter::order::sinc4, 64, 2);
    dfsdm::filter::link_channel(dfsdm_filter, dfsdm_channel);

    dfsdm::configure_clock_output(dfsdm::clk_out_src::apb2, 2400000, true);

    dfsdm::filter::enable_dma(dfsdm_filter, const_cast<int16_t*>(samples_buffer), buffer_length, dfsdm_data_ready);
    dfsdm::channel::enable(dfsdm_channel, true);
    dfsdm::filter::enable(dfsdm_filter, true);
    dfsdm::filter::trigger(dfsdm_filter);

    debug_led->set(true);

    while (true)
    {
        std::cout << "RMS: " << rms << std::endl;
        hal::delay::delay_ms(50);
    }
}
