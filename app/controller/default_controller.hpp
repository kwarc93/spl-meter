/*
 * default_controller.hpp
 *
 *  Created on: 5 paź 2021
 *      Author: kwarc
 */

#ifndef CONTROLLER_DEFAULT_CONTROLLER_HPP_
#define CONTROLLER_DEFAULT_CONTROLLER_HPP_

#include <vector>

#include <app/model/meter.hpp>
#include <app/view/view_interface.hpp>

#include <hal/hal_microphone.hpp>

namespace spl
{
    class default_controller
    {
    public:
        default_controller(std::vector<view_interface*> &views);
        ~default_controller();
        void process(void);
    private:
        void spl_meter_new_data_callback(const spl::meter::data &spl_data);
        void process_user_command(view_interface::user_cmd cmd);

        hal::microphones::digital_mic microphone;

        meter spl_meter;

        std::vector<view_interface*> views;
    };
}

#endif /* CONTROLLER_DEFAULT_CONTROLLER_HPP_ */
