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

#include <hal/hal_microphone.hpp>
#include <cmsis/arm_math.h>

#include "weighting_filter.hpp"

namespace spl
{

class meter
{
public:
    enum class weighting
    {
        A, C, Z
    };

    struct data
    {
        float32_t db;
        float32_t db_max;
        float32_t db_min;
    };

    typedef std::function<void(const data &spl_data)> new_data_cb_t;
public:
    meter(hal::microphone &microphone, new_data_cb_t new_data_cb);
    ~meter();

    void process(void);
    const data &get_data(void);
    void set_weighting(weighting weighting);
private:
    data spl_data;
    new_data_cb_t new_spl_data_cb;

    hal::microphone &microphone;
    void microphone_data_ready(const int16_t *data, uint16_t data_len);

    spl::weighting_filter *weighting_filter;

    std::vector<float32_t> dsp_buffer;
    volatile bool dsp_buffer_ready;
};

}

#endif /* MODEL_METER_HPP_ */
