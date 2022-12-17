/*
 * user_input_interface.hpp
 *
 *  Created on: 17 gru 2022
 *      Author: kwarc
 */

#ifndef CONTROLLER_USER_INPUT_INTERFACE_HPP_
#define CONTROLLER_USER_INPUT_INTERFACE_HPP_

namespace spl
{

class user_input_interface
{
public:
    virtual ~user_input_interface() {};

    enum class user_cmd
    {
        none,
        reset_data,
        change_weighting,
        set_a_weighting,
        set_c_weighting,
        set_z_weighting,
        change_averaging,
        set_fast_averaging,
        set_slow_averaging,
        show_max,
        show_min,
        show_actual,
    };

    virtual user_cmd process(void) = 0;
protected:
    user_cmd last_cmd;
};

}


#endif /* CONTROLLER_USER_INPUT_INTERFACE_HPP_ */
