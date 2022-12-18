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

#include "app/utils.hpp"

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

    lcd_view::user_cmd switch_weighting(char current_weighting)
    {
        /* Loop through: A -> C -> Z */
        switch (current_weighting)
        {
            case 'A':
                return lcd_view::user_cmd::set_c_weighting;
            case 'C':
                return lcd_view::user_cmd::set_z_weighting;
            case 'Z':
                return lcd_view::user_cmd::set_a_weighting;
            default:
                return lcd_view::user_cmd::set_a_weighting;
        }
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
    this->lcd.set_bar(spl::utils::to_bar_level(this->current_data.spl, 4));
}

//-----------------------------------------------------------------------------
/* public */

lcd_view::lcd_view() :
    center_btn {hal::buttons::center_btn()},
    up_btn {hal::buttons::up_btn()},
    down_btn {hal::buttons::down_btn()},
    left_btn {hal::buttons::left_btn()},
    right_btn {hal::buttons::right_btn()}
{
    this->last_cmd = user_cmd::none;
    this->update(view_mode::spl);
}

lcd_view::~lcd_view()
{

}

void lcd_view::update(view_mode view)
{
    if (this->current_view_mode == view)
        return;

    this->current_view_mode = view;
    this->update_lcd(nullptr);
}

void lcd_view::update(const data &data)
{
    this->current_data = data;
    this->update_lcd(&data);
}

lcd_view::user_cmd lcd_view::process(void)
{
    /* TODO: Move button debouncing to timer ISR */
    this->center_btn.debounce();
    this->up_btn.debounce();
    this->down_btn.debounce();
    this->left_btn.debounce();
    this->right_btn.debounce();

    user_cmd cmd = user_cmd::none;

    /* Check user input */
    if (this->center_btn.was_pressed())
    {
        if (this->get_current_view_mode() != view_mode::spl)
        {
            this->update(view_mode::spl);
            return cmd;
        }

        cmd = switch_weighting(this->current_data.weighting);

        this->update(view_mode::spl);
    }

    if (this->up_btn.was_pressed())
    {
        cmd = user_cmd::reset_data;
        this->update(view_mode::max);
    }

    if (this->down_btn.was_pressed())
    {
        cmd = user_cmd::reset_data;
        this->update(view_mode::min);
    }

    if (this->left_btn.was_pressed())
    {
        if (this->get_current_view_mode() == view_mode::spl)
            cmd = user_cmd::set_slow_averaging;
    }

    if (this->right_btn.was_pressed())
    {
        if (this->get_current_view_mode() == view_mode::spl)
            cmd = user_cmd::set_fast_averaging;
    }

    this->last_cmd = cmd;
    return cmd;
}
