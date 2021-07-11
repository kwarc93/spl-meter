/*
 * dfsdm.hpp
 *
 *  Created on: 11 lip 2021
 *      Author: kwarc
 */

#ifndef STM32L4_DFSDM_HPP_
#define STM32L4_DFSDM_HPP_

#include <cstdint>

namespace drivers
{

//--------------------------------------------------------------------------------

class dfsdm final
{
public:
    dfsdm() = delete;

    enum class channel
    {
        CH0, CH1, CH2, CH3, CH4, CH5, CH6, CH7
    };

    enum class filter
    {
        F0, F1, F2, F3
    };

    enum class clk_out_src
    {
        APB2_CLK, SYS_CLK, SAI1_CLK
    };

    static void enable(void);
    static void disable(void);
    static uint32_t configure_clock_output(bool enabled, clk_out_src clk_src, uint32_t frequency);
    static bool configure_channel();
    static bool configure_filter();
    static bool configure_dma();
    static bool link_filter_to_channel();
};

//--------------------------------------------------------------------------------

}

#endif /* STM32L4_DFSDM_HPP_ */
