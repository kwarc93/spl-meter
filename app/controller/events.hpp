/*
 * events.hpp
 *
 *  Created on: 19 gru 2022
 *      Author: kwarc
 */

#ifndef CONTROLLER_EVENTS_HPP_
#define CONTROLLER_EVENTS_HPP_

#include <variant>
#include <functional>

#include <app/model/data_types.hpp>

namespace spl
{

struct change_averaging_evt_t
{
    spl::averaging_t averaging;
};

struct change_weighting_evt_t
{
    spl::weighting_t weighting;
};

struct clear_min_spl_data_evt_t
{

};

struct clear_max_spl_data_evt_t
{

};

using event_t = std::variant<change_averaging_evt_t,
                             change_weighting_evt_t,
                             clear_min_spl_data_evt_t,
                             clear_max_spl_data_evt_t>;

typedef std::function<void(const event_t &e)> event_cb_t;

}


#endif /* CONTROLLER_EVENTS_HPP_ */
