/*
 * view_interface.hpp
 *
 *  Created on: 3 paź 2021
 *      Author: kwarc
 */

#ifndef VIEW_VIEW_INTERFACE_HPP_
#define VIEW_VIEW_INTERFACE_HPP_

#include <app/controller/events.hpp>

namespace spl
{
    /* Forward declaration */
    class default_controller;

    class view_interface
    {
    public:
        virtual ~view_interface() {};

        struct data
        {
            float spl;
            float max_spl;
            float min_spl;
            char weighting;
            char averaging;
        };

        virtual void update(const data &data) = 0;
        virtual void process(void) = 0;

        void set_event_sender_callback(const event_cb_t &cb) { this->send_event_cb = cb; };
    protected:
        data current_data;
        event_cb_t send_event_cb;
    };
}

#endif /* VIEW_VIEW_INTERFACE_HPP_ */
