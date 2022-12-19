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

namespace spl
{
    class default_controller
    {
    public:
        default_controller(meter &model, std::vector<view_interface*> &views);
        ~default_controller();
        void process(void);
    private:
        void model_new_data_callback(const spl::meter::data &data);
        void process_user_command(view_interface::user_cmd cmd);

        meter *model;
        std::vector<view_interface*> views;
    };
}

#endif /* CONTROLLER_DEFAULT_CONTROLLER_HPP_ */
