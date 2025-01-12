/*
 * weighting_filter.hpp
 *
 *  Created on: 1 paź 2021
 *      Author: kwarc
 */

#ifndef MODEL_WEIGHTING_FILTER_HPP_
#define MODEL_WEIGHTING_FILTER_HPP_

#include <vector>

#include <cmsis/arm_math.h>

namespace spl
{

//---------------------------------------------------------------------------
/* Strategy pattern */

class weighting_filter
{
public:
    virtual ~weighting_filter() {};
    virtual bool process(std::vector<float32_t> &input, std::vector<float32_t> &output) = 0;
};

//---------------------------------------------------------------------------

class a_weighting : public weighting_filter
{
public:
    a_weighting();
    bool process(std::vector<float32_t> &input, std::vector<float32_t> &output);
private:
    arm_biquad_cascade_df2T_instance_f32 iir_filter;
    static constexpr uint32_t iir_stages = 3;
    static constexpr float32_t coeffs[5 * iir_stages] = // Fs: 46875 Hz
    {
        0.240209840,  -0.480385663,   0.240175825,   0.201110797,  -0.010111388,
        1.000000000,  -2.000141613,   1.000141623,   1.891429991,  -0.892780174,
        1.000000000,   2.000000000,   1.000000000,   1.994485381,  -0.994492984
    };
    float32_t iir_state[2 * iir_stages];
};

class c_weighting : public weighting_filter
{
public:
    c_weighting();
    bool process(std::vector<float32_t> &input, std::vector<float32_t> &output);
private:
    arm_biquad_cascade_df2T_instance_f32 iir_filter;
    static constexpr uint32_t iir_stages = 2;
    static constexpr float32_t coeffs[5 * iir_stages] = // Fs: 46875 Hz
    {
        0.203135364,   0.406270729,   0.203135364,   0.201110797,  -0.010111388,
        1.000000000,  -2.000000000,   1.000000000,   1.994485381,  -0.994492984
    };
    float32_t iir_state[2 * iir_stages];
};

class z_weighting : public weighting_filter
{
public:
    z_weighting();
    bool process(std::vector<float32_t> &input, std::vector<float32_t> &output);
};

}

#endif /* MODEL_WEIGHTING_FILTER_HPP_ */
