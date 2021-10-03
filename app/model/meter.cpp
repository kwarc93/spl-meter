/*
 * meter.cpp
 *
 *  Created on: 1 paź 2021
 *      Author: kwarc
 */

#include <memory>
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
        asm volatile ("BKPT 0");
    }

    if (this->dsp_buffer.size() != data_len)
    {
        /* Error !*/
        asm volatile ("BKPT 0");
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
    this->dsp_buffer = std::vector<float32_t>(this->mic_samples / 2);
    this->dsp_buffer_ready = false;

    this->mic_data_buffer = std::vector<int16_t>(this->mic_samples);
    this->mic.init(this->mic_data_buffer, std::bind(&meter::mic_data_ready, this, std::placeholders::_1, std::placeholders::_2));

    this->spl_data = {};
    this->new_spl_data_cb = new_data_cb;
    this->spl_data_period = static_cast<float32_t>(this->dsp_buffer.size()) / this->mic.get_sampling_frequency();

    this->spl_data.weighting = weighting::A;
    this->weighting_filter = new a_weighting();
    this->averaging_filter = new slow_averaging(this->spl_data_period);
    this->averaging_time_point = hal::system::clock::time_point();

    this->mic.enable();
}

meter::~meter()
{
    delete this->weighting_filter;
    delete this->averaging_filter;
}

void meter::process(void)
{
    if (this->dsp_buffer_ready)
    {
        /* Apply weighting filter */
        auto dsp_buffer_filtered = std::make_unique<std::vector<float32_t>>(this->dsp_buffer.size());
        if (dsp_buffer_filtered != nullptr)
            this->weighting_filter->process(this->dsp_buffer, *dsp_buffer_filtered);

        this->dsp_buffer_ready = false;

        /* Delete offset */
        float32_t mean;
        arm_mean_f32(dsp_buffer_filtered->data(), dsp_buffer_filtered->size(), &mean);
        arm_offset_f32(dsp_buffer_filtered->data(), -mean, dsp_buffer_filtered->data(), dsp_buffer_filtered->size());

        /* Calculate RMS value */
        float32_t rms;
        arm_rms_f32(dsp_buffer_filtered->data(), dsp_buffer_filtered->size(), &rms);

        /* Normalize RMS value */
        rms /= INT16_MAX;

        /* Calculate dB SPL */
        float32_t db_spl_raw = 94 - this->mic.get_sensitivity() + 20.0f * log10f(rms);

        /* Apply averaging */
        float32_t db_spl = this->averaging_filter->process(db_spl_raw);

        uint32_t averaging_period = this->averaging_filter->time_constant * 1000;
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

const meter::data & meter::get_data(void)
{
    return this->spl_data;
}

void meter::reset_data(void)
{
    this->spl_data = {};
}

void meter::set_weighting(weighting weighting)
{
    switch (weighting)
    {
        case weighting::A:
            this->reset_data();
            delete this->weighting_filter;
            this->weighting_filter = new a_weighting();
            break;
        case weighting::C:
            this->reset_data();
            delete this->weighting_filter;
            this->weighting_filter = new c_weighting();
            break;
        case weighting::Z:
            this->reset_data();
            delete this->weighting_filter;
            this->weighting_filter = new z_weighting();
            break;
        default:
            break;
    }

    this->spl_data.weighting = weighting;
}

void meter::set_averaging(averaging averaging)
{
    switch (averaging)
    {
        case averaging::fast:
            this->reset_data();
            delete this->averaging_filter;
            this->averaging_filter = new fast_averaging(this->spl_data_period);
            break;
        case averaging::slow:
            this->reset_data();
            delete this->averaging_filter;
            this->averaging_filter = new slow_averaging(this->spl_data_period);
            break;
        default:
            break;
    }
}
