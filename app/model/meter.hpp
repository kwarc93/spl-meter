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
#include <memory>

#include <hal/hal_system.hpp>
#include <hal/hal_microphone.hpp>

#include <cmsis/arm_math.h>

#include "data_types.hpp"
#include "weighting_filter.hpp"
#include "averaging_filter.hpp"

namespace spl
{

class meter
{
public:
    typedef std::function<void(const data_t &spl_data)> new_data_cb_t;
public:
    meter(hal::microphone &microphone, const new_data_cb_t &new_data_cb = nullptr);
    ~meter();

    void enable(void);
    void process(void);
    void set_new_data_callback(const new_data_cb_t &new_data_cb);

    const data_t &get_data(void);
    void reset_min_spl_data(void);
    void reset_max_spl_data(void);
    void set_weighting(weighting_t weighting);
    void set_averaging(averaging_t averaging);
private:
    data_t spl_data;
    float32_t spl_data_period;
    new_data_cb_t new_spl_data_cb;

    hal::microphone &mic;
    std::vector<int16_t> mic_data_buffer;
    void mic_data_ready(const int16_t *data, uint16_t data_len);

    std::unique_ptr<spl::weighting_filter> weighting_filter;
    std::unique_ptr<spl::averaging_filter> averaging_filter;
    hal::system::clock::time_point averaging_time_point;

    std::vector<float32_t> dsp_buffer;
    std::vector<float32_t> aux_dsp_buffer;
    volatile bool dsp_buffer_ready;
};

}

#endif /* MODEL_METER_HPP_ */
