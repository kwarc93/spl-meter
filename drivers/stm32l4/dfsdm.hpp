/*
 * dfsdm.hpp
 *
 *  Created on: 11 lip 2021
 *      Author: kwarc
 */

#ifndef STM32L4_DFSDM_HPP_
#define STM32L4_DFSDM_HPP_

#include <cstdint>
#include <cstddef>


namespace drivers
{
//--------------------------------------------------------------------------------
/* TODO: Refactor this class to be non-static, add templates for channel & filter classes */

class dfsdm final
{
public:
    dfsdm() = delete;

    typedef void (*data_ready_callback_t)(const int16_t *data, uint16_t data_len);

    enum class clk_out_src
    {
        apb2, sysclk, sai1
    };

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

        static void enable(id ch, bool state);
        static void configure(id ch, data_pack dp, data_input di, clk_src clk, protocol proto);
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

        enum class order
        {
            fast_sinc, sinc1, sinc2, sinc3, sinc4, sinc5
        };

        static void enable(id f, bool state);
        static void enable_dma(id f, int16_t *data_buffer, uint16_t data_buffer_len, data_ready_callback_t data_ready_cb);
        static void configure(id f, order ord, uint16_t decim, uint8_t avg, bool continous_mode = true, bool fast_mode = true, bool sync_with_f0 = false);
        static void link_channel(id f, channel::id ch);
        static void trigger(id f);
        static int32_t read(id f);
    } filter;

    static void enable(bool state);
    static uint32_t configure_clock_output(clk_out_src clk_src, uint32_t frequency, bool enabled);
    static void dma_handler(filter::id f);

private:

    struct output_data
    {
        int16_t *buffer;
        uint16_t buffer_len;
        data_ready_callback_t ready_callback;
    };

    inline static output_data output_data[4] = {0};

    static void global_toggle(bool state);
};

//--------------------------------------------------------------------------------

}

#endif /* STM32L4_DFSDM_HPP_ */
