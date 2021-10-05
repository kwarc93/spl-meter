/*
 * main.c
 *
 *  Created on: 1 cze 2021
 *      Author: kwarc
 */

#include <iostream>

#include <hal/hal_system.hpp>
#include <hal/hal_led.hpp>

#include <app/controller/default_controller.hpp>

int main(void)
{
    hal::system::init();

    std::cout << "System started" << std::endl;

    auto led = hal::leds::debug();
    bool led_state = true;
    led.set(led_state);

    auto controller = spl::default_controller();

    auto led_blink_start = hal::system::clock::now();

    while (true)
    {
        controller.process();

        if (hal::system::clock::is_elapsed(led_blink_start, std::chrono::milliseconds(500)))
        {
            led_blink_start = hal::system::clock::now();
            led.set(led_state ^= true);
        }
    }
}
