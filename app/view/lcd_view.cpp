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

#include <app/utils.hpp>

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

    const uint8_t max_bar_lvl = 4;

    this->lcd.write(unit + value);
    this->lcd.set_bar(spl::utils::to_bar_level(this->current_data.spl, max_bar_lvl));
}

void lcd_view::update_view_mode(view_mode view)
{
    if (this->current_view_mode == view)
        return;

    this->current_view_mode = view;
    this->update_lcd(nullptr);
}

void lcd_view::send_event(const event_t &e)
{
    if (this->send_event_cb)
        this->send_event_cb(e);
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
    this->update_view_mode(view_mode::spl);
}

lcd_view::~lcd_view()
{

}

void lcd_view::update(const data &data)
{
    this->current_data = data;
    this->update_lcd(&data);
}

void lcd_view::process(void)
{
    /* TODO: Move button debouncing to timer ISR */
    this->center_btn.debounce();
    this->up_btn.debounce();
    this->down_btn.debounce();
    this->left_btn.debounce();
    this->right_btn.debounce();

    /* Check user input */
    if (this->center_btn.was_pressed())
    {
        if (this->current_view_mode == view_mode::spl)
        {
            change_weighting_evt_t e;
            e.weighting = utils::switch_weighting(this->current_data.weighting);
            this->send_event_cb(e);
            return;
        }

        this->update_view_mode(view_mode::spl);
    }

    if (this->up_btn.was_pressed())
    {
        clear_max_spl_data_evt_t e;
        this->send_event_cb(e);
        this->update_view_mode(view_mode::max);
    }

    if (this->down_btn.was_pressed())
    {
        clear_min_spl_data_evt_t e;
        this->send_event_cb(e);
        this->update_view_mode(view_mode::min);
    }

    if (this->left_btn.was_pressed())
    {
        if (this->current_view_mode == view_mode::spl)
        {
            change_averaging_evt_t e;
            e.averaging = spl::averaging_t::slow;
            this->send_event_cb(e);
        }
    }

    if (this->right_btn.was_pressed())
    {
        if (this->current_view_mode == view_mode::spl)
        {
            change_averaging_evt_t e;
            e.averaging = spl::averaging_t::fast;
            this->send_event_cb(e);
        }
    }
}
