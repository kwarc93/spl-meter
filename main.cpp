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
#include <functional>
#include <cmath>

#include <cmsis/arm_math.h>

#include <hal/hal_system.hpp>
#include <hal/hal_delay.hpp>
#include <hal/hal_led.hpp>
#include <hal/hal_microphone.hpp>
#include <hal/hal_lcd.hpp>

#include <app/model/meter.hpp>
#include <app/view/lcd_view.hpp>

static spl::meter *spl_meter;
static spl::lcd_view *spl_view;

void spl_meter_new_data_callback(const spl::meter::data &spl_data)
{
    /* Translate to view data type */
    char weighting;
    switch (spl_data.weighting)
    {
        case spl::meter::weighting::A:
            weighting = 'A';
            break;
        case spl::meter::weighting::C:
            weighting = 'C';
            break;
        case spl::meter::weighting::Z:
            weighting = 'Z';
            break;
        default:
            weighting = ' ';
            break;
    }

    const spl::view_interface::data view_data =
    {
        .spl = spl_data.spl,
        .max_spl = spl_data.spl_max,
        .min_spl = spl_data.spl_min,
        .weighting = weighting,
    };

    spl_view->update(view_data);

    /* Write to STD output or add console view */
    std::cout << "dBA: " << lround(spl_data.spl) << std::endl;
    std::cout << "dBA MAX: " << lround(spl_data.spl_max) << std::endl;
    std::cout << "dBA MIN: " << lround(spl_data.spl_min) << std::endl;
}

int main(void)
{
    hal::system::init();

    std::cout << "System started" << std::endl;

    auto led = hal::leds::debug();
    bool led_state = true;
    led.set(led_state);

    auto microphone = hal::microphones::digital_mic();

    spl_view = new spl::lcd_view();
    spl_view->show(spl::view_interface::view::spl);

    spl_meter = new spl::meter(microphone, spl_meter_new_data_callback);
    spl_meter->set_averaging(spl::meter::averaging::fast);

    auto led_blink_start = hal::system::clock::now();

    while (true)
    {
        spl_meter->process();

        if (hal::system::clock::is_elapsed(led_blink_start, std::chrono::milliseconds(500)))
        {
            led_blink_start = hal::system::clock::now();
            led.set(led_state ^= true);
        }

    }
}
