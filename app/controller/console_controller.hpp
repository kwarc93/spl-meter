/*
 * console_controller.hpp
 *
 *  Created on: 16 gru 2022
 *      Author: kwarc
 */

#ifndef CONTROLLER_CONSOLE_CONTROLLER_HPP_
#define CONTROLLER_CONSOLE_CONTROLLER_HPP_

#include "controller.hpp"

namespace spl
{

class console_controller : public controller
{
public:
    console_controller(view_interface *view);
    void process(void) override;
private:
};

}

#endif /* CONTROLLER_CONSOLE_CONTROLLER_HPP_ */
