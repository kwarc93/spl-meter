/*
 * hal_microphone.hpp
 *
 *  Created on: 1 paź 2021
 *      Author: kwarc
 */

#ifndef HAL_MICROPHONE_HPP_
#define HAL_MICROPHONE_HPP_

#include <hal/hal_interface.hpp>

#include <drivers/mic_mp34dt01.hpp>

namespace hal
{

//---------------------------------------------------------------------------

    class microphone
    {
    public:
        microphone(hal::interface::microphone *interface);
        virtual ~microphone();
        bool init(const hal::interface::microphone::data_ready_cb_t &data_ready_cb);
        void enable(void);
        void disable(void);
        void set_gain(float gain);
        int32_t get_sensitivity(void);
        uint32_t get_sampling_frequency(void);
    protected:
        hal::interface::microphone *interface;
    private:
    };

//-----------------------------------------------------------------------------

namespace microphones
{
    class digital_mic : public microphone
    {
    public:
        digital_mic(void) : microphone {&drv} {}
    private:
        static constexpr auto channel = drivers::dfsdm::channel::id::ch2;
        static constexpr auto filter = drivers::dfsdm::filter::id::f0;
        drivers::mic_mp34dt01 drv {channel, filter};
    };

    /* TODO */
//    class analog_mic : public microphone
//    {
//
//    };
}

}

//---------------------------------------------------------------------------

#endif /* HAL_MICROPHONE_HPP_ */
