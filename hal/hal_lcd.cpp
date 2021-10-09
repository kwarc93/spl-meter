/*
 * hal_lcd.cpp
 *
 *  Created on: 3 paź 2021
 *      Author: kwarc
 */

#include "hal_lcd.hpp"

using namespace hal;

lcd::lcd_6x14::lcd_6x14()
{

}

lcd::lcd_6x14::~lcd_6x14()
{

}

bool lcd::lcd_6x14::write(const std::string_view &s)
{
    return this->drv.write(s);
}

bool lcd::lcd_6x14::set_bar(uint8_t level)
{
    return this->drv.set_bar(level);
}

