/*
 * hal.hpp
 *
 *  Created on: 20 paź 2020
 *      Author: kwarc
 */

#ifndef HAL_SYSTEM_HPP_
#define HAL_SYSTEM_HPP_

#include <chrono>
#include <cstdint>

#include <cmsis/stm32l4xx.h>

namespace hal::system
{
    static constexpr uint32_t msi_clock = 48000000;
    static constexpr uint32_t hsi_clock = 16000000;
    static constexpr uint32_t hse_clock = 8000000;
    static constexpr uint32_t system_clock = msi_clock;
    static constexpr uint32_t systick_freq = 1000;
    extern volatile  uint32_t systick;

    void init(void);

    /* Custom implementation of steady_clock */
    struct clock
    {
        typedef std::chrono::milliseconds duration;
        typedef duration::rep rep;
        typedef duration::period period;
        typedef std::chrono::time_point<clock, duration> time_point;

        static constexpr bool is_steady = true;

        static time_point now(void) noexcept
        {
            return time_point{ duration{ systick } };
        }

        template<class rep, class period>
        static bool is_elapsed(time_point start, const std::chrono::duration<rep, period>& duration)
        {
            return (start + duration) < now();
        }
    };
}

#endif /* HAL_SYSTEM_HPP_ */
