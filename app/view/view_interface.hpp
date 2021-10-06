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
        };

        enum class view
        {
            spl, max_spl, min_spl, all
        };

        virtual void show(view view) = 0;
        virtual void update(const data &data) = 0;
        view get_current_view(void) { return current_view; };
    protected:
        data current_data;
        view current_view;
    };
}

#endif /* VIEW_VIEW_INTERFACE_HPP_ */
