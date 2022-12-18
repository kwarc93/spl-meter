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
            char averaging;
        };

        enum class view_mode
        {
            spl, max, min, all
        };

        enum class user_cmd
        {
            none,
            reset_data,
            set_a_weighting,
            set_c_weighting,
            set_z_weighting,
            set_fast_averaging,
            set_slow_averaging,
        };

        virtual void update(view_mode view) = 0;
        virtual void update(const data &data) = 0;
        virtual user_cmd process(void) = 0;
        view_mode get_current_view_mode(void) { return current_view_mode; };
    protected:
        user_cmd last_cmd;
        data current_data;
        view_mode current_view_mode;
    };
}

#endif /* VIEW_VIEW_INTERFACE_HPP_ */
