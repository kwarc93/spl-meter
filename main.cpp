/*
 * main.c
 *
 *  Created on: 1 cze 2021
 *      Author: kwarc
 */

#include <cstdio>

#include <hal/hal_system.hpp>
#include <hal/hal_led.hpp>

#include <app/controller/button_controller.hpp>
#include <app/controller/console_controller.hpp>

#include <app/view/lcd_view.hpp>
#include <app/view/console_view.hpp>

int main(void)
{
    hal::system::init();

    printf("System started\n");

    auto led = hal::leds::debug();
    bool led_state = true;
    led.set(led_state);

    auto view = spl::console_view();
    auto controller = spl::button_controller(&view);

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
