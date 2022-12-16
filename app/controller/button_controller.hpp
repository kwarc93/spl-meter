/*
 * button_controller.hpp
 *
 *  Created on: 16 gru 2022
 *      Author: kwarc
 */

#ifndef CONTROLLER_BUTTON_CONTROLLER_HPP_
#define CONTROLLER_BUTTON_CONTROLLER_HPP_

#include "controller.hpp"

#include <hal/hal_button.hpp>

namespace spl
{

class button_controller : public controller
{
public:
    button_controller(view_interface *view);
    void process(void) override;
private:
    hal::buttons::center_btn center_btn;
    hal::buttons::up_btn up_btn;
    hal::buttons::down_btn down_btn;
    hal::buttons::left_btn left_btn;
    hal::buttons::right_btn right_btn;
};

}


#endif /* CONTROLLER_BUTTON_CONTROLLER_HPP_ */
