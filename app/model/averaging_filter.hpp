/*
 * averaging_filter.hpp
 *
 *  Created on: 2 paź 2021
 *      Author: kwarc
 */

#ifndef MODEL_AVERAGING_FILTER_HPP_
#define MODEL_AVERAGING_FILTER_HPP_

#include <cmath>

#include <cmsis/arm_math.h>

namespace spl
{

//---------------------------------------------------------------------------

class averaging_filter
{
public:
    averaging_filter(float32_t time_constant, float32_t dt);
    virtual ~averaging_filter();
    float32_t process(float32_t input);
    float32_t time_constant;
private:
    float32_t alpha;
    float32_t output;
};

class fast_averaging : public averaging_filter
{
public:
    fast_averaging(float32_t dt) : averaging_filter(0.125f, dt) {};
};

class slow_averaging : public averaging_filter
{
public:
    slow_averaging(float32_t dt) : averaging_filter(1.0f, dt) {};
};

//---------------------------------------------------------------------------

}

#endif /* MODEL_AVERAGING_FILTER_HPP_ */
