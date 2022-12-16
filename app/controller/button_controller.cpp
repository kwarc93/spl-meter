/*
 * button_controller.cpp
 *
 *  Created on: 16 gru 2022
 *      Author: kwarc
 */

#include "button_controller.hpp"

using namespace spl;

//-----------------------------------------------------------------------------
/* public */

button_controller::button_controller(view_interface *view) : controller {view},
    center_btn {hal::buttons::center_btn()},
    up_btn {hal::buttons::up_btn()},
    down_btn {hal::buttons::down_btn()},
    left_btn {hal::buttons::left_btn()},
    right_btn {hal::buttons::right_btn()}
{

}

void button_controller::process(void)
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
        if (this->spl_view->get_current_view_mode() != view_interface::view_mode::spl)
        {
            this->spl_view->update(view_interface::view_mode::spl);
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

        this->spl_view->update(view_interface::view_mode::spl);
    }

    if (this->up_btn.was_pressed())
    {
        this->spl_meter.reset_data();
        this->spl_view->update(view_interface::view_mode::max);
    }

    if (this->down_btn.was_pressed())
    {
        this->spl_meter.reset_data();
        this->spl_view->update(view_interface::view_mode::min);
    }


    if (this->left_btn.was_pressed())
    {
        if (this->spl_view->get_current_view_mode() == view_interface::view_mode::spl)
            this->spl_meter.set_averaging(meter::averaging::slow);
    }

    if (this->right_btn.was_pressed())
    {
        if (this->spl_view->get_current_view_mode() == view_interface::view_mode::spl)
            this->spl_meter.set_averaging(meter::averaging::fast);
    }
}


