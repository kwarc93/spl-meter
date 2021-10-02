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

meter::meter(hal::microphone &microphone, new_data_cb_t new_data_cb) : mic {microphone}
{
    this->new_spl_data_cb = new_data_cb;
    this->weighting_filter = new a_weighting();

    this->dsp_buffer = std::vector<float32_t>(2048);
    this->dsp_buffer_ready = false;

    this->mic_data_buffer = std::vector<int16_t>(4096);
    this->mic.init(this->mic_data_buffer, std::bind(&meter::mic_data_ready, this, std::placeholders::_1, std::placeholders::_2));
    this->mic.enable();
}

meter::~meter()
{
    delete this->weighting_filter;
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
        static uint32_t sum_cnt = 0;
        static float32_t db_spl_sum = 0;
        db_spl_sum += 94 - this->mic.get_sensitivity() + 20.0f * log10f(rms);
        sum_cnt++;

        if (sum_cnt >= 4)
        {
            float32_t db_spl = db_spl_sum / sum_cnt;
            sum_cnt = 0;
            db_spl_sum = 0;

            this->spl_data.db_max = std::max(db_spl, this->spl_data.db_max);
            this->spl_data.db_max = std::min(db_spl, this->spl_data.db_min);
            this->spl_data.db = db_spl;

            if (this->new_spl_data_cb != nullptr)
                this->new_spl_data_cb(this->spl_data);
        }
    }
}
const meter::data & meter::get_data(void)
{
    return this->spl_data;
}

void meter::set_weighting(weighting weighting)
{
    delete this->weighting_filter;

    switch (weighting)
    {
        case weighting::A:
            this->weighting_filter = new a_weighting();
            break;
        case weighting::C:
            this->weighting_filter = new c_weighting();
            break;
        case weighting::Z:
            this->weighting_filter = new z_weighting();
            break;
        default:
            this->weighting_filter = nullptr;
            break;
    }
}
