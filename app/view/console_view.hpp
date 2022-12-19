/*
 * console_view.hpp
 *
 *  Created on: 3 paź 2021
 *      Author: kwarc
 */

#ifndef VIEW_CONSOLE_VIEW_HPP_
#define VIEW_CONSOLE_VIEW_HPP_

#include "view_interface.hpp"

#include <cstdint>

namespace spl
{

class console_view : public view_interface
{
public:
    console_view();
    ~console_view();

    void update(const data &data) override;
    void process(void) override;
};

}


#endif /* VIEW_CONSOLE_VIEW_HPP_ */
