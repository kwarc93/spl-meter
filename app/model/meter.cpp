/*
 * meter.cpp
 *
 *  Created on: 1 paź 2021
 *      Author: kwarc
 */

#include <cmath>
#include <algorithm>
#include <functional>

#include <app/model/meter.hpp>

using namespace spl;

//---------------------------------------------------------------------------
/* private */

/* warning: This function is called from ISR */
void meter::mic_data_ready(const int16_t *data, uint16_t data_len)
{
    if (this->dsp_buffer_ready)
    {
        /* Overrun !*/
//        asm volatile ("BKPT 0");
    }

    if (this->dsp_buffer.size() != data_len)
    {
        /* Error !*/
//        asm volatile ("BKPT 0");
    }

    /* Copy integer values to DSP buffer */
    for (uint32_t i = 0; i < data_len; i++)
        this->dsp_buffer[i] = data[i];

    this->dsp_buffer_ready = true;
}

//---------------------------------------------------------------------------
/* public */

meter::meter(hal::microphone &microphone, const new_data_cb_t &new_data_cb) : mic {microphone}
{
    this->mic_data_buffer.assign(4096, 0);
    this->mic.init(this->mic_data_buffer, std::bind(&meter::mic_data_ready, this, std::placeholders::_1, std::placeholders::_2));

    this->dsp_buffer.assign(this->mic_data_buffer.size() / 2, 0);
    this->aux_dsp_buffer.assign(this->mic_data_buffer.size() / 2, 0);
    this->dsp_buffer_ready = false;

    this->new_spl_data_cb = new_data_cb;
    this->spl_data.spl_min = 120;
    this->spl_data.spl_max = 0;
    this->spl_data_period = static_cast<float32_t>(this->dsp_buffer.size()) / this->mic.get_sampling_frequency();

    this->spl_data.weighting = weighting_t::a;
    this->weighting_filter = new a_weighting();

    this->spl_data.averaging = averaging_t::slow;
    this->averaging_filter = new slow_averaging(this->spl_data_period, this->spl_data.spl);
}

meter::~meter()
{
    delete this->weighting_filter;
    delete this->averaging_filter;
}

void meter::enable(void)
{
    this->averaging_time_point = hal::system::clock::time_point();
    this->mic.enable();
}

void meter::process(void)
{
    /* DSP processing takes around 2.68 msec at 48MHz */
    if (this->dsp_buffer_ready)
    {
        auto &current_dsp_buffer = this->dsp_buffer;

        /* Apply weighting filter */
        this->weighting_filter->process(current_dsp_buffer, this->aux_dsp_buffer);
        this->dsp_buffer_ready = false;

        current_dsp_buffer = this->aux_dsp_buffer;

        /* Delete offset */
        float32_t mean;
        arm_mean_f32(current_dsp_buffer.data(), current_dsp_buffer.size(), &mean);
        arm_offset_f32(current_dsp_buffer.data(), -mean, current_dsp_buffer.data(), current_dsp_buffer.size());

        /* Calculate RMS value */
        float32_t rms;
        arm_rms_f32(current_dsp_buffer.data(), current_dsp_buffer.size(), &rms);

        /* Normalize RMS value */
        rms /= INT16_MAX;

        /* Calculate dB SPL */
        float32_t db_spl_raw = 94 - this->mic.get_sensitivity() + 20.0f * log10f(rms);

        /* Apply averaging */
        float32_t db_spl = this->averaging_filter->process(db_spl_raw);

        const uint32_t averaging_period = this->averaging_filter->time_constant * 1000;

        if (hal::system::clock::is_elapsed(this->averaging_time_point, std::chrono::milliseconds(averaging_period)))
        {
            this->averaging_time_point = hal::system::clock::now();

            this->spl_data.spl_max = std::max(db_spl, this->spl_data.spl_max);
            this->spl_data.spl_min = std::min(db_spl, this->spl_data.spl_min);
            this->spl_data.spl = db_spl;

            if (this->new_spl_data_cb != nullptr)
                this->new_spl_data_cb(this->spl_data);
        }
    }
}

void meter::set_new_data_callback(const new_data_cb_t &new_data_cb)
{
    this->new_spl_data_cb = new_data_cb;
}

const spl::data_t & meter::get_data(void)
{
    return this->spl_data;
}

void meter::reset_min_spl_data(void)
{
    this->spl_data.spl_min = this->spl_data.spl;
}

void meter::reset_max_spl_data(void)
{
    this->spl_data.spl_max = this->spl_data.spl;
}

void meter::set_weighting(weighting_t weighting)
{
    switch (weighting)
    {
        case weighting_t::a:
            delete this->weighting_filter;
            this->weighting_filter = new a_weighting();
            break;
        case weighting_t::c:
            delete this->weighting_filter;
            this->weighting_filter = new c_weighting();
            break;
        case weighting_t::z:
            delete this->weighting_filter;
            this->weighting_filter = new z_weighting();
            break;
        default:
            return;
    }

    this->spl_data.weighting = weighting;

    if (this->new_spl_data_cb != nullptr)
        this->new_spl_data_cb(this->spl_data);
}

void meter::set_averaging(averaging_t averaging)
{
    switch (averaging)
    {
        case averaging_t::fast:
            delete this->averaging_filter;
            this->averaging_filter = new fast_averaging(this->spl_data_period, this->spl_data.spl);
            break;
        case averaging_t::slow:
            delete this->averaging_filter;
            this->averaging_filter = new slow_averaging(this->spl_data_period, this->spl_data.spl);
            break;
        default:
            return;
    }

    this->spl_data.averaging = averaging;

    if (this->new_spl_data_cb != nullptr)
        this->new_spl_data_cb(this->spl_data);
}
