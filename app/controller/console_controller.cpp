/*
 * console_controller.cpp
 *
 *  Created on: 16 gru 2022
 *      Author: kwarc
 */

#include "console_controller.hpp"

#include <cstdio>

using namespace spl;

//-----------------------------------------------------------------------------
/* public */

console_controller::console_controller(view_interface *view) : controller {view}
{

}

void console_controller::process(void)
{
    this->spl_meter.process();

    /* TODO: Add control for keyboard input */
}

