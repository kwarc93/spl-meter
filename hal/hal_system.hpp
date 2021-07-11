/*
 * hal.hpp
 *
 *  Created on: 20 paź 2020
 *      Author: kwarc
 */

#ifndef HAL_SYSTEM_HPP_
#define HAL_SYSTEM_HPP_

#include <cmsis/stm32l4xx.h>

namespace hal::system
{
    static constexpr uint32_t msi_clock = 48000000;
    static constexpr uint32_t hsi_clock = 16000000;
    static constexpr uint32_t hse_clock = 8000000;
    static constexpr uint32_t system_clock = msi_clock;
    static constexpr uint32_t systick_freq = 1000;

    void init(void);
}


#endif /* HAL_SYSTEM_HPP_ */
