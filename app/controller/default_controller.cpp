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
        case spl::meter::weighting::A:
            weighting = 'A';
            break;
        case spl::meter::weighting::C:
            weighting = 'C';
            break;
        case spl::meter::weighting::Z:
            weighting = 'Z';
            break;
        default:
            weighting = ' ';
            break;
    }

    const spl::view_interface::data view_data =
    {
        .spl = spl_data.spl,
        .max_spl = spl_data.spl_max,
        .min_spl = spl_data.spl_min,
        .weighting = weighting,
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
    this->spl_view.show(view_interface::view::spl);
    this->spl_meter.set_averaging(meter::averaging::fast);
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

    /* TODO: Complete this logic */
    if (this->center_btn.was_pressed())
    {
        this->spl_view.show(view_interface::view::spl);
    }

    if (this->up_btn.was_pressed())
    {
        this->spl_meter.reset_data();
        this->spl_view.show(view_interface::view::max_spl);
    }

    if (this->down_btn.was_pressed())
    {
        this->spl_meter.reset_data();
        this->spl_view.show(view_interface::view::min_spl);
    }

    if (this->left_btn.was_pressed())
    {
        this->spl_meter.set_averaging(meter::averaging::slow);
    }

    if (this->right_btn.was_pressed())
    {
        this->spl_meter.set_averaging(meter::averaging::fast);
    }
}
