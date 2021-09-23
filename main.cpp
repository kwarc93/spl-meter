/*
 * main.c
 *
 *  Created on: 1 cze 2021
 *      Author: kwarc
 */

#include <iostream>
#include <string>
#include <array>
#include <cmath>

#include <hal/hal_system.hpp>
#include <hal/hal_delay.hpp>
#include <hal/hal_led.hpp>

#include <drivers/lcd_gh08172.hpp>
#include <drivers/mic_mp34dt01.hpp>

#include <cmsis/arm_math.h>

static std::array<volatile float32_t, 2048> dsp_samples_buffer;
static volatile bool data_ready = false;

void mic_data_ready(const int16_t *data, uint16_t data_len)
{
    if (data_ready)
    {
        /* Overrun !*/
        asm volatile ("BKPT 0");
    }

    /* Copy integer values to DSP buffer */
    for (uint32_t i = 0; i < data_len; i++)
        dsp_samples_buffer[i] = data[i];

    data_ready = true;
}

int main(void)
{
    hal::system::init();

    std::cout << "System started" << std::endl;

    /* LCD driver test */
    auto lcd = new drivers::lcd_gh08172();
    uint8_t spl_value = 95;
    uint8_t padding = spl_value / 100 ? 0 : spl_value / 10 ? 1 : 2;
    const std::string lcd_str = "A:" + std::string(padding, ' ') + std::to_string(spl_value) + "dB";
    lcd->write(lcd_str);

    /* Digital microphone driver test*/
    auto microphone = new drivers::mic_mp34dt01(drivers::dfsdm::channel::id::ch2, drivers::dfsdm::filter::id::f0);
    microphone->init(mic_data_ready);
    microphone->enable();

    std::cout << "Microphone fs: " << microphone->get_sampling_frequency() << "Hz" << std::endl;

    auto debug_led = new hal::leds::debug();
    debug_led->set(true);

    while (true)
    {
        if (data_ready)
        {
            float32_t *dsp_buffer = const_cast<float32_t*>(dsp_samples_buffer.data());
            uint32_t data_len = dsp_samples_buffer.size();

            /* Delete offset */
            float32_t mean;
            arm_mean_f32(dsp_buffer, data_len, &mean);
            arm_offset_f32(dsp_buffer, -mean, dsp_buffer, data_len);

            /* Normalize signal */
            const float32_t scale = 1.0f / INT16_MAX;
            arm_scale_f32(dsp_buffer, scale, dsp_buffer, data_len);

            /* Calculate RMS value of normalized signal */
            float32_t rms;
            arm_rms_f32(dsp_buffer, data_len, const_cast<float32_t*>(&rms));

            /* Calculate DB SPL */
            float db_spl = microphone->get_aop() + 20.0f * log10f(rms);
            std::cout << "dB SPL: " << db_spl << std::endl;

            data_ready = false;
        }
    }
}
