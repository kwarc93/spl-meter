/*
 * controller_interface.cpp
 *
 *  Created on: 5 paź 2021
 *      Author: kwarc
 */

#include <functional>

#include "controller.hpp"

using namespace spl;

//-----------------------------------------------------------------------------
/* public */

controller::controller(view_interface *view) :
    spl_view {view},
    microphone {hal::microphones::digital_mic()},
    spl_meter {meter(microphone, std::bind(&controller::spl_meter_new_data_callback, this, std::placeholders::_1))}
{
    this->spl_view->update(view_interface::view_mode::spl);
}

//-----------------------------------------------------------------------------
/* private */

void controller::spl_meter_new_data_callback(const spl::meter::data &spl_data)
{
    if (this->spl_view == nullptr)
        return;

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

    this->spl_view->update(view_data);
}
