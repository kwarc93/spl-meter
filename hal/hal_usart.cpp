/*
 * hal_usart.cpp
 *
 *  Created on: 24 paź 2020
 *      Author: kwarc
 */

#include "hal_usart.hpp"

#include <drivers/stm32l4/usart.hpp>

using namespace hal;

interface::serial & usart::debug::get_instance(void)
{
    static drivers::usart usart2 { drivers::usart::id::usart2, 115200 };
    return usart2;
}
