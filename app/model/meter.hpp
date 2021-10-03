/*
 * meter.hpp
 *
 *  Created on: 1 paź 2021
 *      Author: kwarc
 */

#ifndef MODEL_METER_HPP_
#define MODEL_METER_HPP_

#include <vector>
#include <functional>

#include <hal/hal_system.hpp>
#include <hal/hal_microphone.hpp>

#include <cmsis/arm_math.h>

#include "weighting_filter.hpp"
#include "averaging_filter.hpp"

namespace spl
{

class meter
{
public:
    enum class weighting
    {
        A, C, Z
    };

    enum class averaging
    {
        fast, slow
    };

    struct data
    {
        float32_t spl;
        float32_t spl_max;
        float32_t spl_min;
        meter::weighting weighting;
    };

    typedef std::function<void(const data &spl_data)> new_data_cb_t;
public:
    meter(hal::microphone &microphone, const new_data_cb_t &new_data_cb);
    ~meter();

    void process(void);
    const data &get_data(void);
    void reset_data(void);
    void set_weighting(weighting weighting);
    void set_averaging(averaging averaging);
private:
    data spl_data;
    float32_t spl_data_period;
    new_data_cb_t new_spl_data_cb;

    hal::microphone &mic;
    const uint32_t mic_samples = 4096;
    std::vector<int16_t> mic_data_buffer;
    void mic_data_ready(const int16_t *data, uint16_t data_len);

    spl::weighting_filter *weighting_filter;
    spl::averaging_filter *averaging_filter;
    hal::system::clock::time_point averaging_time_point;

    std::vector<float32_t> dsp_buffer;
    volatile bool dsp_buffer_ready;
};

}

#endif /* MODEL_METER_HPP_ */
