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

#include <cmsis/arm_math.h>

#include <hal/hal_system.hpp>
#include <hal/hal_delay.hpp>
#include <hal/hal_led.hpp>

#include <drivers/lcd_gh08172.hpp>
#include <drivers/mic_mp34dt01.hpp>

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

    /* Digital microphone driver test*/
    auto microphone = new drivers::mic_mp34dt01(drivers::dfsdm::channel::id::ch2, drivers::dfsdm::filter::id::f0);
    microphone->init(mic_data_ready);
    microphone->enable();

    std::cout << "Microphone fs: " << microphone->get_sampling_frequency() << "Hz" << std::endl;

    /* LED driver test*/
    auto led = new hal::leds::debug();
    bool led_state = true;
    auto led_blink_start = hal::system::clock::now();
    led->set(led_state);

    while (true)
    {
        if (hal::system::clock::is_elapsed(led_blink_start, std::chrono::milliseconds(500)))
        {
            led_blink_start = hal::system::clock::now();
            led->set(led_state ^= true);
        }

        if (data_ready)
        {
            float32_t *dsp_buffer = const_cast<float32_t*>(dsp_samples_buffer.data());
            uint32_t data_len = dsp_samples_buffer.size();

            /* Delete offset */
            float32_t mean;
            arm_mean_f32(dsp_buffer, data_len, &mean);
            arm_offset_f32(dsp_buffer, -mean, dsp_buffer, data_len);

            /* Calculate RMS value */
            float32_t rms;
            arm_rms_f32(dsp_buffer, data_len, &rms);

            /* Normalize RMS value */
            rms /= INT16_MAX;

            /* Calculate DB SPL */
            static uint32_t sum_cnt = 0;
            static float32_t db_spl_sum = 0;
            db_spl_sum += microphone->get_aop() + 20.0f * log10f(rms) + 3.0f;
            sum_cnt++;

            if (sum_cnt >= 4)
            {
                uint32_t db_spl = db_spl_sum / sum_cnt;

                /* Write to LCD */
                uint8_t padding = db_spl / 100 ? 0 : db_spl / 10 ? 1 : 2;
                const std::string lcd_str = "dBZ:" + std::string(padding, ' ') + std::to_string(db_spl);
                lcd->write(lcd_str);

                /* Write to STD output */
                std::cout << "dBZ: " << db_spl_sum / sum_cnt << std::endl;

                sum_cnt = 0;
                db_spl_sum = 0;
            }

            data_ready = false;
        }
    }
}
