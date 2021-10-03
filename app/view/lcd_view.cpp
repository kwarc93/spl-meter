/*
 * lcd_view.cpp
 *
 *  Created on: 3 paź 2021
 *      Author: kwarc
 */

#include "lcd_view.hpp"

#include <cmath>
#include <string>

#include <hal/hal_lcd.hpp>

using namespace spl;

//-----------------------------------------------------------------------------
/* private */

namespace
{
    inline std::string to_aligned_string(uint8_t value)
    {
        uint8_t padding = value / 100 ? 0 : value / 10 ? 1 : 2;
        return std::string(padding, ' ') + std::to_string(value);
    }
}

void lcd_view::update_lcd(void)
{
    std::string_view s;

    switch (this->curent_view)
    {
        case view::spl:
            s = std::string(1, this->current_data.weighting) + ":" + to_aligned_string(lround(this->current_data.spl)) + "dB";
            break;
        case view::max_spl:
            s = "MAX:" + to_aligned_string(lround(this->current_data.max_spl));
            break;
        case view::min_spl:
            s = "MIN:" + to_aligned_string(lround(this->current_data.min_spl));
            break;
        case view::all:
        default:
            /* Not supported */
            return;
    }

    this->lcd.write(s);
}

//-----------------------------------------------------------------------------
/* public */

lcd_view::lcd_view()
{

}

lcd_view::~lcd_view()
{

}

void lcd_view::show(view view)
{
    this->curent_view = view;
    this->update_lcd();
}

void lcd_view::update(const data &data)
{
    this->current_data = data;
    this->update_lcd();
}
