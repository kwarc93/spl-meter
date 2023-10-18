/*
 * view_interface.hpp
 *
 *  Created on: 3 paź 2021
 *      Author: kwarc
 */

#ifndef VIEW_VIEW_INTERFACE_HPP_
#define VIEW_VIEW_INTERFACE_HPP_

#include <app/controller/events.hpp>
#include <middlewares/observer.hpp>

namespace spl
{
    class view_interface : public middlewares::subject<event_t>
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
            int error_code;
        };

        virtual void update(const data &data) = 0;
        virtual void process(void) = 0;

    protected:
        data current_data;
    };
}

#endif /* VIEW_VIEW_INTERFACE_HPP_ */
