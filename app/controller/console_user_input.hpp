/*
 * console_user_input.hpp
 *
 *  Created on: 17 gru 2022
 *      Author: kwarc
 */

#ifndef CONTROLLER_CONSOLE_USER_INPUT_HPP_
#define CONTROLLER_CONSOLE_USER_INPUT_HPP_

#include "user_input_interface.hpp"

namespace spl
{

class console_user_input : public user_input_interface
{
public:
    console_user_input();
    ~console_user_input();

    user_cmd process(void) override;
};

}

#endif /* CONTROLLER_CONSOLE_USER_INPUT_HPP_ */
