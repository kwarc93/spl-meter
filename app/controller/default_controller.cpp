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

void default_controller::process_user_command(view_interface::user_cmd cmd)
{
    if (cmd == view_interface::user_cmd::none)
        return;

    /* Handle model-related commands */
    switch (cmd)
    {
        case view_interface::user_cmd::reset_data:
            this->spl_meter.reset_data();
            break;

        case view_interface::user_cmd::set_a_weighting:
            this->spl_meter.set_weighting(meter::weighting::a);
            break;

        case view_interface::user_cmd::set_c_weighting:
            this->spl_meter.set_weighting(meter::weighting::c);
            break;

        case view_interface::user_cmd::set_z_weighting:
            this->spl_meter.set_weighting(meter::weighting::z);
            break;

        case view_interface::user_cmd::set_slow_averaging:
            this->spl_meter.set_averaging(meter::averaging::slow);
            break;

        case view_interface::user_cmd::set_fast_averaging:
            this->spl_meter.set_averaging(meter::averaging::fast);
            break;

        default:
            break;
    }
}

//-----------------------------------------------------------------------------
/* public */

default_controller::default_controller(std::vector<view_interface*> &views) :
    microphone {hal::microphones::digital_mic()},
    spl_meter {meter(microphone, std::bind(&default_controller::spl_meter_new_data_callback, this, std::placeholders::_1))},
    views {views}
{

}

default_controller::~default_controller(void)
{

}

void default_controller::process(void)
{
    this->spl_meter.process();

    for (auto view : this->views)
    {
        if (view != nullptr)
        {
            const view_interface::user_cmd cmd = view->process();
            this->process_user_command(cmd);
        }
    }
}
