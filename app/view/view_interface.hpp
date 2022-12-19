/*
 * view_interface.hpp
 *
 *  Created on: 3 paź 2021
 *      Author: kwarc
 */

#ifndef VIEW_VIEW_INTERFACE_HPP_
#define VIEW_VIEW_INTERFACE_HPP_

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

        void set_controller(default_controller *controller) { this->controller = controller; };
        virtual void update(const data &data) = 0;
        virtual void process(void) = 0;
    protected:
        data current_data;
        default_controller *controller;
    };
}

#endif /* VIEW_VIEW_INTERFACE_HPP_ */
