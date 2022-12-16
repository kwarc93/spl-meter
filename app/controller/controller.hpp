/*
 * default_controller.hpp
 *
 *  Created on: 5 paź 2021
 *      Author: kwarc
 */

#ifndef CONTROLLER_DEFAULT_CONTROLLER_HPP_
#define CONTROLLER_DEFAULT_CONTROLLER_HPP_

#include <app/model/meter.hpp>
#include <app/view/view_interface.hpp>

#include <hal/hal_microphone.hpp>

namespace spl
{

//---------------------------------------------------------------------------
/* Strategy pattern */

class controller
{
public:
    controller(view_interface *view);
    virtual ~controller() {};
    virtual void process(void) = 0;
protected:
    void spl_meter_new_data_callback(const spl::meter::data &spl_data);

    view_interface *spl_view;
    hal::microphones::digital_mic microphone;
    meter spl_meter;
};

}

#endif /* CONTROLLER_DEFAULT_CONTROLLER_HPP_ */
