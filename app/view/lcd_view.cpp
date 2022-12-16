/*
 * lcd_view.cpp
 *
 *  Created on: 3 paź 2021
 *      Author: kwarc
 */

#include "lcd_view.hpp"

#include <cmath>
#include <string>
#include <utility>

#include <hal/hal_lcd.hpp>

using namespace spl;

//-----------------------------------------------------------------------------
/* private */

namespace
{
    inline std::string to_aligned_string(float value)
    {
        const uint32_t value_int = lroundf(value);
        const uint8_t padding = value_int / 100 ? 0 : value_int / 10 ? 1 : 2;
        return std::string(padding, ' ') + std::to_string(value_int);
    }

    uint8_t to_bar_level(float value)
    {
        constexpr uint8_t min = 30;
        constexpr uint8_t max = 120;
        constexpr uint8_t diff = max - min;

        uint8_t bar_lvl = lroundf(value);

        return (4 * ((bar_lvl < min ? min : bar_lvl) - min) + diff / 2) / diff;
    }
}

void lcd_view::update_lcd(const data *data)
{
    std::string unit, value;

    switch (this->current_view_mode)
    {
        case view_mode::spl:
            unit = "L" + std::string(1, this->current_data.weighting) + std::string(1, this->current_data.averaging) + ":";
            value = to_aligned_string(this->current_data.spl);
            break;
        case view_mode::max:
            unit = "MAX:";
            value = to_aligned_string(this->current_data.max_spl);
            break;
        case view_mode::min:
            unit = "MIN:";
            value = to_aligned_string(this->current_data.min_spl);
            break;
        case view_mode::all:
        default:
            /* Not supported */
            return;
    }

    if (data == nullptr)
        value = "   ";

    this->lcd.write(unit + value);
    this->lcd.set_bar(to_bar_level(this->current_data.spl));
}

//-----------------------------------------------------------------------------
/* public */

lcd_view::lcd_view()
{

}

lcd_view::~lcd_view()
{

}

void lcd_view::update(view_mode view)
{
    this->current_view_mode = view;
    this->update_lcd(nullptr);
}

void lcd_view::update(const data &data)
{
    this->current_data = data;
    this->update_lcd(&data);
}
