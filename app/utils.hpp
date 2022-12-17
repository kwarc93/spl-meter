/*
 * utils.hpp
 *
 *  Created on: 17 gru 2022
 *      Author: kwarc
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <cmath>
#include <cstdint>

namespace spl::utils
{

static uint8_t to_bar_level(float spl_value, uint8_t max_bar_lvl)
{
    constexpr uint8_t spl_min = 30;
    constexpr uint8_t spl_max = 120;
    constexpr uint8_t diff = spl_max - spl_min;

    uint8_t bar_lvl = lroundf(spl_value);

    return (max_bar_lvl * ((bar_lvl < spl_min ? spl_min : bar_lvl) - spl_min) + diff / 2) / diff;
}

}

#endif /* UTILS_HPP_ */
