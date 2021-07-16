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

    /* DFSDM test */
    using drivers::dfsdm;
    dfsdm::enable();
    dfsdm::configure_clock_output(dfsdm::clk_out_src::apb2, 2400000, true);
    dfsdm::channel::set_offset(dfsdm::channel::id::ch2, 0);
    dfsdm::channel::set_bitshift(dfsdm::channel::id::ch2, 16);
    dfsdm::channel::configure(dfsdm::channel::id::ch2,
                              dfsdm::channel::data_pack::standard,
                              dfsdm::channel::data_input::ext_pin,
                              dfsdm::channel::clk_src::int_ckout,
                              dfsdm::channel::protocol::spi_r_edge,
                              true);

    while (true)
    {
        debug_led->set(true);
        hal::delay::delay_ms(500);
        debug_led->set(false);
        hal::delay::delay_ms(500);
    }
}
