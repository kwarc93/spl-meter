/*
 * default_controller.hpp
 *
 *  Created on: 5 paź 2021
 *      Author: kwarc
 */

#ifndef CONTROLLER_DEFAULT_CONTROLLER_HPP_
#define CONTROLLER_DEFAULT_CONTROLLER_HPP_

#include <vector>

#include <app/model/data_types.hpp>
#include <app/view/view_interface.hpp>

#include <middlewares/observer.hpp>

#include "events.hpp"

namespace spl
{
    /* Forward declaration */
    class meter;

    class default_controller : public middlewares::observer<event_t>
    {
    public:
        default_controller(meter &model, std::vector<view_interface*> &views);
        ~default_controller();

        void process(void);
    private:
        void update(const event_t &data) override;

        /* Event handlers */
        void event_handler(const change_averaging_evt_t &e);
        void event_handler(const change_weighting_evt_t &e);
        void event_handler(const clear_min_spl_data_evt_t &e);
        void event_handler(const clear_max_spl_data_evt_t &e);

        void model_new_data_callback(const spl::data_t &data);

        meter *model;
        std::vector<view_interface*> views;

        int error_code;
    };
}

#endif /* CONTROLLER_DEFAULT_CONTROLLER_HPP_ */
