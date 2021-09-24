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

    struct clock
    {
        using duration = std::chrono::duration<std::uint32_t, std::milli>;
        using time_point = std::chrono::time_point<clock>;
        static constexpr bool is_steady = true;

        static time_point now(void) noexcept
        {
            return time_point{ duration{ systick } };
        }

        template<class Rep, class Period>
        static bool is_elapsed(time_point start, const std::chrono::duration<Rep, Period>& duration)
        {
            return (start + duration) < now();
        }
    };
}

#endif /* HAL_SYSTEM_HPP_ */
