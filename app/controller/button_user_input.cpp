/*
 * button_user_input.cpp
 *
 *  Created on: 17 gru 2022
 *      Author: kwarc
 */

#include "button_user_input.hpp"

using namespace spl;

//-----------------------------------------------------------------------------
/* private */

//-----------------------------------------------------------------------------
/* public */

button_user_input::button_user_input() :
    center_btn {hal::buttons::center_btn()},
    up_btn {hal::buttons::up_btn()},
    down_btn {hal::buttons::down_btn()},
    left_btn {hal::buttons::left_btn()},
    right_btn {hal::buttons::right_btn()}
{
    this->last_cmd = user_input_interface::user_cmd::none;
}

button_user_input::~button_user_input()
{

}

user_input_interface::user_cmd button_user_input::process(void)
{
    /* TODO: Move button debouncing to timer ISR */
    this->center_btn.debounce();
    this->up_btn.debounce();
    this->down_btn.debounce();
    this->left_btn.debounce();
    this->right_btn.debounce();

    user_input_interface::user_cmd cmd = user_input_interface::user_cmd::none;

    /* Check user input */
    if (this->center_btn.was_pressed())
        cmd = user_input_interface::user_cmd::show_actual;

    if (this->up_btn.was_pressed())
        cmd = user_input_interface::user_cmd::show_max;

    if (this->down_btn.was_pressed())
        cmd = user_input_interface::user_cmd::show_min;

    if (this->left_btn.was_pressed())
        cmd = user_input_interface::user_cmd::set_slow_averaging;

    if (this->right_btn.was_pressed())
        cmd = user_input_interface::user_cmd::set_fast_averaging;

    this->last_cmd = cmd;
    return cmd;
}
