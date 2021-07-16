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

    enum class clk_out_src
    {
        apb2, sysclk, sai1
    };

    static void enable(void);
    static void disable(void);
    static uint32_t configure_clock_output(clk_out_src clk_src, uint32_t frequency, bool enabled);

    static class channel
    {
    public:
        channel() = delete;

        enum class id
        {
            ch0, ch1, ch2, ch3, ch4, ch5, ch6, ch7
        };

        enum class data_pack
        {
            standard, interleaved, dual
        };

        enum class data_input
        {
            ext_pin, int_adc, int_register,
        };

        enum class clk_src
        {
            ext_pin, int_ckout, int_ckout_2nd_f_edge, int_ckout_2nd_r_edge
        };

        enum class protocol
        {
            spi_r_edge, spi_f_edge, manch_r0f1, manch_r1f0
        };

        static void configure(id ch, data_pack dp, data_input di, clk_src clk, protocol proto, bool enabled);
        static void set_offset(id ch, uint32_t offset);
        static void set_bitshift(id ch, uint8_t right_shift);
    } channel;

    static class filter
    {
    public:
        filter() = delete;

        enum class id
        {
            f0, f1, f2, f3
        };

        void configure(void);
    } filter;

private:
    static void global_toggle(bool state);
//    static bool configure_filter();
//    static bool configure_dma();
//    static bool link_filter_to_channel();
};

//--------------------------------------------------------------------------------

}

#endif /* STM32L4_DFSDM_HPP_ */
