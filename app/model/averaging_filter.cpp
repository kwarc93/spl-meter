/*
 * averaging_filter.cpp
 *
 *  Created on: 2 paź 2021
 *      Author: kwarc
 */

#include "averaging_filter.hpp"

using namespace spl;

averaging_filter::averaging_filter(float32_t time_constant, float32_t dt)
{
    this->time_constant = time_constant;
    this->alpha = 1 - expf(-dt / time_constant);
    this->output = 0;
}

averaging_filter::~averaging_filter()
{

}

float32_t averaging_filter::process(float32_t input)
{
    return output = (1 - this->alpha) * output + this->alpha * input;
}
