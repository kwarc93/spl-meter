/*
 * main.c
 *
 *  Created on: 1 cze 2021
 *      Author: kwarc
 */

#include <iostream>

#include <hal/hal_system.hpp>
#include <hal/hal_delay.hpp>
#include <hal/hal_led.hpp>
#include <drivers/lcd_gh08172.hpp>

int main(void)
{
    hal::system::init();

    std::cout << "System started" << std::endl;

    auto lcd = new drivers::lcd_gh08172();
    auto debug_led = new hal::leds::debug();

    lcd->write("/HELLO");

    while (true)
    {
        debug_led->set(true);
        hal::delay::delay_ms(500);
        debug_led->set(false);
        hal::delay::delay_ms(500);
    }
}
