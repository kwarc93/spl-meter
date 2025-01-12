/*
 * mic_mp34dt01.hpp
 *
 *  Created on: 22 wrz 2021
 *      Author: kwarc
 */

#ifndef MIC_MP34DT01_HPP_
#define MIC_MP34DT01_HPP_

#include <hal/hal_interface.hpp>

#include <drivers/stm32l4/dfsdm.hpp>

namespace drivers
{
    class mic_mp34dt01 : public hal::interface::microphone
    {
    public:
        mic_mp34dt01(drivers::dfsdm::channel::id ch, drivers::dfsdm::filter::id f);

        void init(std::vector<int16_t> &data_buffer, const hal::interface::microphone::data_ready_cb_t &data_ready_cb);
        void enable(void);
        void disable(void);
        void set_gain(float gain);
        int32_t get_sensitivity(void);
        uint32_t get_sampling_frequency(void);

    private:
        static constexpr uint32_t aop = 120;            /* 120dB SPL */
        static constexpr uint32_t snr = 61;             /* 61dB */
        static constexpr int32_t sensitivity = -26;     /* -26dbFS */
        static constexpr uint32_t clock = 3000000;      /* 3MHz */

        const drivers::dfsdm::channel::id channel;
        drivers::dfsdm::filter::id filter;
        uint32_t sampling_frequency;
    };
}

#endif /* MIC_MP34DT01_HPP_ */
