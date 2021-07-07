/*
 * lcd_gh08172.cpp
 *
 *  Created on: 27 cze 2021
 *      Author: kwarc
 */

#include "lcd_gh08172.hpp"

#include <drivers/stm32l4/lcd.hpp>
#include <drivers/stm32l4/delay.hpp>

#include <hal/hal_lcd.hpp>

#include <array>

using namespace drivers;

//-----------------------------------------------------------------------------
/* private */

void lcd_gh08172::set_character(uint16_t value, uint8_t position)
{
    if (position >= this->positions)
        return;

    const uint8_t bit_map[this->positions] = {0, 2, 4, 6, 8, 10};

    for (auto &com : hal::lcd::com_map)
    {
        drivers::lcd::ram[com] |= ((uint64_t)!!(value & (1 << 12)) << hal::lcd::segment_map[bit_map[position]]) |
                                  ((uint64_t)!!(value & (1 << 13)) << hal::lcd::segment_map[bit_map[position] + 1]) |
                                  ((uint64_t)!!(value & (1 << 14)) << hal::lcd::segment_map[23 - bit_map[position] - 1]) |
                                  ((uint64_t)!!(value & (1 << 15)) << hal::lcd::segment_map[23 - bit_map[position]]);
        value <<= 4;
    }
}

//-----------------------------------------------------------------------------
/* public */

lcd_gh08172::lcd_gh08172()
{
    /* TODO: Move GPIO initialization to low level LCD driver? */
    for (const auto &pin : hal::lcd::gpio)
        gpio::init(pin, gpio::af::af11, gpio::mode::af);

    drivers::lcd::init();

    /* Test all segments */
    for (uint8_t idx = 0; idx < 8; idx++)
        drivers::lcd::ram[idx] = 0xffffffffffffffff;

    drivers::lcd::update();
    drivers::delay::ms(1000);
    drivers::lcd::clear();

    /* 'HELLO' */
    this->set_character(0xFA00, 0);
    this->set_character(0x9D00, 1);
    this->set_character(0x1900, 2);
    this->set_character(0x1900, 3);
    this->set_character(0x5F00, 4);

    drivers::lcd::update();
}

bool lcd_gh08172::write(std::string &s)
{
    return true;
}
