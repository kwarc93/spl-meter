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

    this->spl_view.update(view_data);
}

//-----------------------------------------------------------------------------
/* public */

default_controller::default_controller(void) :
    center_btn {hal::buttons::center_btn()},
    up_btn {hal::buttons::up_btn()},
    down_btn {hal::buttons::down_btn()},
    left_btn {hal::buttons::left_btn()},
    right_btn {hal::buttons::right_btn()},
    microphone {hal::microphones::digital_mic()},
    spl_view {lcd_view()},
    spl_meter {meter(microphone, std::bind(&default_controller::spl_meter_new_data_callback, this, std::placeholders::_1))}

{
    this->spl_view.update(view_interface::view::spl);
}

default_controller::~default_controller(void)
{

}

void default_controller::process(void)
{
    this->spl_meter.process();

    /* TODO: Move button debouncing to timer ISR */
    this->center_btn.debounce();
    this->up_btn.debounce();
    this->down_btn.debounce();
    this->left_btn.debounce();
    this->right_btn.debounce();

    /* Check user input */
    if (this->center_btn.was_pressed())
    {
        if (this->spl_view.get_current_view() != view_interface::view::spl)
        {
            this->spl_view.update(view_interface::view::spl);
            return;
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

        this->spl_view.update(view_interface::view::spl);
    }

    if (this->up_btn.was_pressed())
    {
        this->spl_meter.reset_data();
        this->spl_view.update(view_interface::view::max);
    }

    if (this->down_btn.was_pressed())
    {
        this->spl_meter.reset_data();
        this->spl_view.update(view_interface::view::min);
    }


    if (this->left_btn.was_pressed())
    {
        if (this->spl_view.get_current_view() == view_interface::view::spl)
            this->spl_meter.set_averaging(meter::averaging::slow);
    }

    if (this->right_btn.was_pressed())
    {
        if (this->spl_view.get_current_view() == view_interface::view::spl)
            this->spl_meter.set_averaging(meter::averaging::fast);
    }
}
