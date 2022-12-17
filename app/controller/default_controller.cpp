/*
 * default_controller.cpp
 *
 *  Created on: 5 paź 2021
 *      Author: kwarc
 */

#include <functional>

#include "default_controller.hpp"

using namespace spl;

//-----------------------------------------------------------------------------
/* private */

void default_controller::spl_meter_new_data_callback(const spl::meter::data &spl_data)
{
    /* Translate to view data type */
    char weighting;
    switch (spl_data.weighting)
    {
        case spl::meter::weighting::a:
            weighting = 'A';
            break;
        case spl::meter::weighting::c:
            weighting = 'C';
            break;
        case spl::meter::weighting::z:
            weighting = 'Z';
            break;
        default:
            weighting = ' ';
            break;
    }

    char averaging;
    switch (spl_data.averaging)
    {
        case spl::meter::averaging::slow:
            averaging = 'S';
            break;
        case spl::meter::averaging::fast:
            averaging = 'F';
            break;
        default:
            averaging = ' ';
            break;
    }

    const spl::view_interface::data view_data =
    {
        .spl = spl_data.spl,
        .max_spl = spl_data.spl_max,
        .min_spl = spl_data.spl_min,
        .weighting = weighting,
        .averaging = averaging,
    };

    for (auto view : this->views)
    {
        if (view != nullptr)
            view->update(view_data);
    }
}

void default_controller::process_user_command(user_input_interface::user_cmd cmd)
{
    if (cmd == user_input_interface::user_cmd::none)
        return;

    /* Handle model-related commands */
    switch (cmd)
    {
        case user_input_interface::user_cmd::reset_data:
            this->spl_meter.reset_data();
            break;

        case user_input_interface::user_cmd::change_weighting:
        {
            const meter::weighting current_weighting = this->spl_meter.get_data().weighting;

            /* Loop through: A -> C -> Z */
            switch (current_weighting)
            {
                case meter::weighting::a:
                    this->spl_meter.set_weighting(meter::weighting::c);
                    break;
                case meter::weighting::c:
                    this->spl_meter.set_weighting(meter::weighting::z);
                    break;
                case meter::weighting::z:
                    this->spl_meter.set_weighting(meter::weighting::a);
                    break;
                default:
                    this->spl_meter.set_weighting(meter::weighting::a);
                    break;
            }

            break;
        }

        case user_input_interface::user_cmd::change_averaging:
        {
            const meter::averaging current_averaging = this->spl_meter.get_data().averaging;

            /* Loop through: Fast -> Slow */
            if (current_averaging == meter::averaging::slow)
                this->spl_meter.set_averaging(meter::averaging::fast);
            else
                this->spl_meter.set_averaging(meter::averaging::slow);
            break;
        }

        case user_input_interface::user_cmd::set_a_weighting:
            this->spl_meter.set_weighting(meter::weighting::a);
            break;

        case user_input_interface::user_cmd::set_c_weighting:
            this->spl_meter.set_weighting(meter::weighting::c);
            break;

        case user_input_interface::user_cmd::set_z_weighting:
            this->spl_meter.set_weighting(meter::weighting::z);
            break;

        case user_input_interface::user_cmd::set_slow_averaging:
            this->spl_meter.set_averaging(meter::averaging::slow);
            break;

        case user_input_interface::user_cmd::set_fast_averaging:
            this->spl_meter.set_averaging(meter::averaging::fast);
            break;

        default:
            break;
    }

    /* Handle view-related commands for all views */
    for (auto view : this->views)
    {
        switch (cmd)
        {
            case user_input_interface::user_cmd::show_max:
                this->spl_meter.reset_data();
                view->update(view_interface::view_mode::max);
                break;

            case user_input_interface::user_cmd::show_min:
                this->spl_meter.reset_data();
                view->update(view_interface::view_mode::min);
                break;

            case user_input_interface::user_cmd::show_actual:
            {
                if (view->get_current_view_mode() != view_interface::view_mode::spl)
                {
                    view->update(view_interface::view_mode::spl);
                    break;
                }

                const meter::weighting current_weighting = this->spl_meter.get_data().weighting;

                switch (current_weighting)
                {
                    case meter::weighting::a:
                        this->spl_meter.set_weighting(meter::weighting::c);
                        break;
                    case meter::weighting::c:
                        this->spl_meter.set_weighting(meter::weighting::z);
                        break;
                    case meter::weighting::z:
                        this->spl_meter.set_weighting(meter::weighting::a);
                        break;
                    default:
                        this->spl_meter.set_weighting(meter::weighting::a);
                        break;
                }

                view->update(view_interface::view_mode::spl);
                break;
            }

            default:
                break;
        }
    }
}

//-----------------------------------------------------------------------------
/* public */

default_controller::default_controller(std::vector<view_interface*> &views, std::vector<user_input_interface*> &user_inputs) :
    microphone {hal::microphones::digital_mic()},
    spl_meter {meter(microphone, std::bind(&default_controller::spl_meter_new_data_callback, this, std::placeholders::_1))},
    views {views},
    user_inputs {user_inputs}
{

}

default_controller::~default_controller(void)
{

}

void default_controller::process(void)
{
    this->spl_meter.process();

    for (auto user_input : this->user_inputs)
    {
        if (user_input != nullptr)
        {
            const user_input_interface::user_cmd cmd = user_input->process();
            this->process_user_command(cmd);
        }
    }
}
