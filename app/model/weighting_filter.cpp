/*
 * weighting_filter.cpp
 *
 *  Created on: 1 paź 2021
 *      Author: kwarc
 */

#include "weighting_filter.hpp"

using namespace spl;

a_weighting::a_weighting()
{
    arm_biquad_cascade_df2T_init_f32(&this->iir_filter, this->iir_stages, this->coeffs, this->iir_state);
}

bool a_weighting::process(std::vector<float32_t> &input, std::vector<float32_t> &output)
{
    if (input.size() != output.size())
        return false;

    arm_biquad_cascade_df2T_f32(&this->iir_filter, input.data(), output.data(), output.size());
    return true;
}

c_weighting::c_weighting()
{
    arm_biquad_cascade_df2T_init_f32(&this->iir_filter, this->iir_stages, this->coeffs, this->iir_state);
}

bool c_weighting::process(std::vector<float32_t> &input, std::vector<float32_t> &output)
{
    if (input.size() != output.size())
        return false;

    arm_biquad_cascade_df2T_f32(&this->iir_filter, input.data(), output.data(), output.size());
    return true;
}

z_weighting::z_weighting()
{

}

bool z_weighting::process(std::vector<float32_t> &input, std::vector<float32_t> &output)
{
    if (input.size() != output.size())
        return false;

    output = input;
    return true;
}
