/*
 * console_user_input.cpp
 *
 *  Created on: 17 gru 2022
 *      Author: kwarc
 */

#include "console_user_input.hpp"

using namespace spl;

//-----------------------------------------------------------------------------
/* private */

//-----------------------------------------------------------------------------
/* public */

console_user_input::console_user_input()
{
    this->last_cmd = user_input_interface::user_cmd::none;
}

console_user_input::~console_user_input()
{

}

user_input_interface::user_cmd console_user_input::process(void)
{
    /* TODO: Handle console/serial port input */
    return user_input_interface::user_cmd::none;
}

