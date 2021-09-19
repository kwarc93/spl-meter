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

const std::map<const char, uint16_t> lcd_gh08172::character_map =
{
    /* Capital letters */
    {'A', 0xFE00},
    {'B', 0x6714},
    {'C', 0x1D00},
    {'D', 0x4714},
    {'E', 0x9D00},
    {'F', 0x9C00},
    {'G', 0x3F00},
    {'H', 0xFA00},
    {'I', 0x0014},
    {'J', 0x5300},
    {'K', 0x9841},
    {'L', 0x1900},
    {'M', 0x5A48},
    {'N', 0x5A09},
    {'O', 0x5F00},
    {'P', 0xFC00},
    {'Q', 0x5F01},
    {'R', 0xFC01},
    {'S', 0xAF00},
    {'T', 0x0414},
    {'U', 0x5B00},
    {'V', 0x18C0},
    {'W', 0x5A81},
    {'X', 0x00C9},
    {'Y', 0x0058},
    {'Z', 0x05C0},

    /* Digits */
    {'0', 0x5F00},
    {'1', 0x4200},
    {'2', 0xF500},
    {'3', 0x6700},
    {'4', 0xEA00},
    {'5', 0xAF00},
    {'6', 0xBF00},
    {'7', 0x4600},
    {'8', 0xFF00},
    {'9', 0xEF00},

    /* Letters */
    {'d', 0xF300},
    {'m', 0xB210},
    //{'n', 0x2210},
    {'n', 0x9001},
    //{'u', 0x6084},
    {'u', 0x0310},

    /* Others */
    {' ', 0x0000},
    {'*', 0xA0DD},
    {'(', 0x0041},
    {')', 0x0088},
    {'+', 0xA000},
    {'-', 0xA014},
    {'/', 0x00C0},
    {'\\', 0x0009},
    {255, 0xFFDD},
};

void lcd_gh08172::set_character(uint16_t value, uint8_t position)
{
    if (position >= this->positions)
        return;

    for (auto &com : hal::lcd::com_map)
    {
        /* Clear */
        drivers::lcd::ram[com] &= ~(((uint64_t)1 << hal::lcd::segment_map[2 * position]) |
                                   ((uint64_t)1 << hal::lcd::segment_map[2 * position + 1]) |
                                   ((uint64_t)1 << hal::lcd::segment_map[23 - 2 * position - 1]) |
                                   ((uint64_t)1 << hal::lcd::segment_map[23 - 2 * position]));
        /* Set */
        drivers::lcd::ram[com] |= ((uint64_t)!!(value & (1 << 12)) << hal::lcd::segment_map[2 * position]) |
                                  ((uint64_t)!!(value & (1 << 13)) << hal::lcd::segment_map[2 * position + 1]) |
                                  ((uint64_t)!!(value & (1 << 14)) << hal::lcd::segment_map[23 - 2 * position - 1]) |
                                  ((uint64_t)!!(value & (1 << 15)) << hal::lcd::segment_map[23 - 2 * position]);
        value <<= 4;
    }
}

void lcd_gh08172::set_bar(bool value, uint8_t bar)
{
    if (bar >= this->bars)
        return;

    constexpr std::array<std::pair<hal::lcd::com, hal::lcd::segment>, 4> bar_map =
    {
        {
            {hal::lcd::com::COM3, hal::lcd::segment::SEG11},
            {hal::lcd::com::COM2, hal::lcd::segment::SEG11},
            {hal::lcd::com::COM3, hal::lcd::segment::SEG9},
            {hal::lcd::com::COM2, hal::lcd::segment::SEG9}
        }
    };

    if (value)
        drivers::lcd::ram[hal::lcd::com_map[bar_map[bar].first]] |= ((uint64_t)value << hal::lcd::segment_map[bar_map[bar].second]);
    else
        drivers::lcd::ram[hal::lcd::com_map[bar_map[bar].first]] &= ~((uint64_t)value << hal::lcd::segment_map[bar_map[bar].second]);
}

//-----------------------------------------------------------------------------
/* public */

lcd_gh08172::lcd_gh08172()
{
    /* TODO: Move GPIO initialization to low level LCD driver? */
    for (const auto &pin : hal::lcd::gpio)
        gpio::configure(pin, gpio::mode::af, gpio::af::af11);

    drivers::lcd::init();

    /* Test all segments */
    for (uint8_t idx = 0; idx < 8; idx++)
        drivers::lcd::ram[idx] = 0xffffffffffffffff;

    drivers::lcd::update();
    drivers::delay::ms(1000);
    drivers::lcd::clear();
}

bool lcd_gh08172::write(const std::string &s)
{
    uint8_t position = 0;
    for (std::size_t i = 0; i < s.length(); i++)
    {
        auto char_map_item = this->character_map.find(s[i]);
        if (char_map_item == this->character_map.end())
        {
            char_map_item = this->character_map.find(std::toupper(s[i]));
            if (char_map_item == this->character_map.end())
            {
                continue;   /* TODO: Or return false? */
            }
        }

        uint16_t lcd_char = char_map_item->second;

        /* Colon or comma cannot be added on  pre-last & last position */
        if (position < this->positions - 2)
        {
            if (s[i + 1] == ',' || s[i + 1] == '.')
                lcd_char |= 0x0002;

            if (s[i + 1] == ':')
                lcd_char |= 0x0020;
        }

        this->set_character(lcd_char, position++);
    }

    drivers::lcd::update();
    return position < this->positions;
}
