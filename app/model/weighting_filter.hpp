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
    arm_biquad_casd_df1_inst_f32 iir_filter;
    static constexpr uint32_t iir_stages = 3;
    static constexpr float32_t coeffs[5 * iir_stages] = {2.8148e-01, -5.6291e-01, 2.8144e-01, 4.3181e-02, -4.6615e-04,
                                                         1.0000e+00, -2.0001e+00, 1.0001e+00, 1.9935e+00, -9.9355e-01,
                                                         1.0000e+00,  2.0000e+00, 1.0000e+00, 1.8737e+00, -8.7551e-01};
    float32_t iir_state[4 * iir_stages];
};

class c_weighting : public weighting_filter
{
public:
    c_weighting();
    bool process(std::vector<float32_t> &input, std::vector<float32_t> &output);
private:
    arm_biquad_casd_df1_inst_f32 iir_filter;
    static constexpr uint32_t iir_stages = 2;
    static constexpr float32_t coeffs[5 * iir_stages] = {2.4025e-01,  4.8051e-01, 2.4025e-01, 4.3181e-02, -4.6615e-04,
                                                         1.0000e+00, -2.0000e+00, 1.0000e+00, 1.9935e+00, -9.9355e-01};
    float32_t iir_state[4 * iir_stages];
};

class z_weighting : public weighting_filter
{
public:
    z_weighting();
    bool process(std::vector<float32_t> &input, std::vector<float32_t> &output);
};

}

#endif /* MODEL_WEIGHTING_FILTER_HPP_ */
