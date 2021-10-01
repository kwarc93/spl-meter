/*
 * main.c
 *
 *  Created on: 1 cze 2021
 *      Author: kwarc
 */

#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <cmath>

#include <cmsis/arm_math.h>

#include <hal/hal_system.hpp>
#include <hal/hal_delay.hpp>
#include <hal/hal_led.hpp>
#include <hal/hal_microphone.hpp>

#include <drivers/lcd_gh08172.hpp>

static std::array<volatile float32_t, 2048> dsp_samples_buffer;
static volatile bool data_ready = false;

void mic_data_ready(const int16_t *data, uint16_t data_len)
{
    if (data_ready)
    {
        /* Overrun !*/
//        asm volatile ("BKPT 0");
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
    auto microphone = new hal::microphones::digital_mic();
    microphone->init(mic_data_ready);
    microphone->enable();

    std::cout << "Microphone fs: " << microphone->get_sampling_frequency() << "Hz" << std::endl;

    /* LED driver test*/
    auto led = new hal::leds::debug();
    bool led_state = true;
    auto led_blink_start = hal::system::clock::now();
    led->set(led_state);

    /* IIR filter init (for A-weighting) */
    arm_biquad_casd_df1_inst_f32 iir;
    const uint32_t stages = 3;
    const float32_t coeffs[5 * stages] = {2.8148e-01, -5.6291e-01, 2.8144e-01, 4.3181e-02, -4.6615e-04,
                                          1.0000e+00, -2.0001e+00, 1.0001e+00, 1.9935e+00, -9.9355e-01,
                                          1.0000e+00,  2.0000e+00, 1.0000e+00, 1.8737e+00, -8.7551e-01};
    float32_t state[4 * stages] = {0};
    arm_biquad_cascade_df1_init_f32(&iir, stages, coeffs, state);

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

            /* Apply A-weighting filter */
            auto dsp_buffer_filtered = std::make_unique<float32_t[]>(data_len);
            if (dsp_buffer_filtered != nullptr)
            {
                arm_biquad_cascade_df1_f32(&iir, dsp_buffer, dsp_buffer_filtered.get(), data_len);
                dsp_buffer = dsp_buffer_filtered.get();
            }

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
            db_spl_sum += 94 - microphone->get_sensitivity() + 20.0f * log10f(rms);
            sum_cnt++;

            if (sum_cnt >= 4)
            {
                uint32_t db_spl = lround(db_spl_sum / sum_cnt);
                sum_cnt = 0;
                db_spl_sum = 0;

                /* Write to LCD */
                uint8_t padding = db_spl / 100 ? 0 : db_spl / 10 ? 1 : 2;
                const std::string_view db_spl_str = "A:" + std::string(padding, ' ') + std::to_string(db_spl) + "dB";
                lcd->write(db_spl_str);

                /* Write to STD output */
                std::cout << db_spl_str << std::endl;
            }

            data_ready = false;
        }
    }
}
