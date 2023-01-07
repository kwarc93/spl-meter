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

#include <hal/hal_interface.hpp>

#include "libs/spsc_queue.hpp"

namespace spl
{

class console_view : public view_interface
{
public:
    console_view();
    ~console_view();

    void update(const data &data) override;
    void process(void) override;
private:
    void character_received_callback(const std::byte *data, std::size_t bytes_read);

    char received_char;
    spsc_queue<char, 16> char_queue;
    hal::interface::serial &stdio_serial;
};

}


#endif /* VIEW_CONSOLE_VIEW_HPP_ */
