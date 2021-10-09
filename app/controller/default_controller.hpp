/*
 * default_controller.hpp
 *
 *  Created on: 5 paź 2021
 *      Author: kwarc
 */

#ifndef CONTROLLER_DEFAULT_CONTROLLER_HPP_
#define CONTROLLER_DEFAULT_CONTROLLER_HPP_

#include <app/model/meter.hpp>
#include <app/view/lcd_view.hpp>

#include <hal/hal_microphone.hpp>
#include <hal/hal_button.hpp>

/* TODO: Add strategy pattern to allow to use multiple controllers. */

namespace spl
{
    class default_controller
    {
    public:
        default_controller(void);
        ~default_controller();
        void process(void);
    private:
        void spl_meter_new_data_callback(const spl::meter::data &spl_data);

        hal::buttons::center_btn center_btn;
        hal::buttons::up_btn up_btn;
        hal::buttons::down_btn down_btn;
        hal::buttons::left_btn left_btn;
        hal::buttons::right_btn right_btn;
        hal::microphones::digital_mic microphone;

        lcd_view spl_view;
        meter spl_meter;
    };
}

#endif /* CONTROLLER_DEFAULT_CONTROLLER_HPP_ */
