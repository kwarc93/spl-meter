/*
 * data_types.hpp
 *
 *  Created on: 19 gru 2022
 *      Author: kwarc
 */

#ifndef MODEL_DATAT_TYPES_HPP_
#define MODEL_DATAT_TYPES_HPP_

#include <cmsis/arm_math.h>

namespace spl
{
    enum class weighting_t
    {
        a, c, z
    };

    enum class averaging_t
    {
        fast, slow
    };

    struct data_t
    {
        float32_t spl;
        float32_t spl_max;
        float32_t spl_min;
        weighting_t weighting;
        averaging_t averaging;
    };
}

#endif /* MODEL_DATAT_TYPES_HPP_ */
