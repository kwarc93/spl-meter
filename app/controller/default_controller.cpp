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

void default_controller::model_new_data_callback(const spl::meter::data &model_data)
{
    /* Translate to view data type */
    char weighting;
    switch (model_data.weighting)
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
    switch (model_data.averaging)
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
        .spl = model_data.spl,
        .max_spl = model_data.spl_max,
        .min_spl = model_data.spl_min,
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
            this->model->reset_data();
            break;

        case view_interface::user_cmd::set_a_weighting:
            this->model->set_weighting(meter::weighting::a);
            break;

        case view_interface::user_cmd::set_c_weighting:
            this->model->set_weighting(meter::weighting::c);
            break;

        case view_interface::user_cmd::set_z_weighting:
            this->model->set_weighting(meter::weighting::z);
            break;

        case view_interface::user_cmd::set_slow_averaging:
            this->model->set_averaging(meter::averaging::slow);
            break;

        case view_interface::user_cmd::set_fast_averaging:
            this->model->set_averaging(meter::averaging::fast);
            break;

        default:
            break;
    }
}

//-----------------------------------------------------------------------------
/* public */

default_controller::default_controller(meter &model, std::vector<view_interface*> &views) :
    model {&model},
    views {views}
{
    this->model->register_new_data_callback(std::bind(&default_controller::model_new_data_callback, this, std::placeholders::_1));
    this->model->initialize();
}

default_controller::~default_controller(void)
{

}

void default_controller::process(void)
{
    this->model->process();

    for (auto view : this->views)
    {
        if (view != nullptr)
        {
            const view_interface::user_cmd cmd = view->process();
            this->process_user_command(cmd);
        }
    }
}
