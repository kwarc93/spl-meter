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
    static constexpr float32_t coeffs[5 * iir_stages] =
    {
        0.28147504346,  -0.56291022628,   0.28143518563,   0.04318103380,  -0.00046615042,
        1.00000000000,  -2.00014161345,   1.00014162348,   1.99353908705,  -0.99354952290,
        1.00000000000,   2.00000000000,   1.00000000000,   1.87367745134,  -0.87551448640
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
    static constexpr float32_t coeffs[5 * iir_stages] =
    {
        0.24025487669,   0.48050975338,   0.24025487669,   0.04318103380,  -0.00046615042,
        1.00000000000,  -2.00000000000,   1.00000000000,   1.99353908697,  -0.99354952282
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
