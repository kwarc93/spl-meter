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

#include <app/model/meter.hpp>

static bool new_spl_data = false;
static uint32_t spl_value = 0;

void spl_meter_new_data_callback(const spl::meter::data &spl_data)
{
    new_spl_data = true;
    spl_value = lround(spl_data.db);
}

int main(void)
{
    hal::system::init();

    std::cout << "System started" << std::endl;

    auto lcd = drivers::lcd_gh08172();
    auto led = hal::leds::debug();
    bool led_state = true;
    led.set(led_state);

    auto microphone = hal::microphones::digital_mic();
    auto spl_meter = spl::meter(microphone, spl_meter_new_data_callback);

    auto led_blink_start = hal::system::clock::now();

    while (true)
    {
        spl_meter.process();

        if (new_spl_data)
        {
            new_spl_data = false;

            /* Write to LCD */
            uint8_t padding = spl_value / 100 ? 0 : spl_value / 10 ? 1 : 2;
            const std::string_view db_spl_str = "A:" + std::string(padding, ' ') + std::to_string(spl_value) + "dB";
            lcd.write(db_spl_str);

            /* Write to STD output */
            std::cout << db_spl_str << std::endl;
        }

        if (hal::system::clock::is_elapsed(led_blink_start, std::chrono::milliseconds(500)))
        {
            led_blink_start = hal::system::clock::now();
            led.set(led_state ^= true);
        }

    }
}
