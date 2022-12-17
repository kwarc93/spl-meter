/*
 * button_user_input.hpp
 *
 *  Created on: 17 gru 2022
 *      Author: kwarc
 */

#ifndef CONTROLLER_BUTTON_USER_INPUT_HPP_
#define CONTROLLER_BUTTON_USER_INPUT_HPP_

#include "user_input_interface.hpp"

#include <hal/hal_button.hpp>

namespace spl
{

class button_user_input : public user_input_interface
{
public:
    button_user_input();
    ~button_user_input();

    user_cmd process(void) override;
private:
    hal::buttons::center_btn center_btn;
    hal::buttons::up_btn up_btn;
    hal::buttons::down_btn down_btn;
    hal::buttons::left_btn left_btn;
    hal::buttons::right_btn right_btn;
};

}


#endif /* CONTROLLER_BUTTON_USER_INPUT_HPP_ */
