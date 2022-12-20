/*
 * console_view.cpp
 *
 *  Created on: 16 gru 2022
 *      Author: kwarc
 */

#include "console_view.hpp"

#include <cmath>
#include <cstdio>

#include <app/controller/default_controller.hpp>
#include <app/utils.hpp>

#include <hal/hal_usart.hpp>

using namespace spl;

/*
 * Console view example:
 *
 * Weighting: A
 * Averaging: Fast
 *
 * L = 41 dB
 * Lmax = 88 dB
 * Lmin = 33 dB
 *
 * |--------     |
 * 0           120dB
 */

#define CLI_START   "\e[12;1H"
#define GUI_END     "\e[10;21H"

//-----------------------------------------------------------------------------
/* private */

void console_view::character_received_callback(const std::byte *data, std::size_t bytes_read)
{
    /* WARNING: This is called from interrupt */

    if (bytes_read == 1)
        this->char_received = true;
}

//-----------------------------------------------------------------------------
/* public */

console_view::console_view() : stdio_serial { hal::usart::stdio::get_instance() }
{
    /* Set coursor at the start of the commandline and save it, clear screen */
    printf(CLI_START "\e[s\e[2J");

    /* Start listening for character */
    this->char_received = false;
    this->stdio_serial.read_async(reinterpret_cast<std::byte*>(&this->received_char), 1,
                                 [this](const std::byte *data, std::size_t bytes_read)
                                 {
                                      this->character_received_callback(data, bytes_read);
                                 }, true);
}

console_view::~console_view()
{

}

void console_view::update(const data &data)
{
    /* Clear screen & reset cursor */
    printf(GUI_END "\e[1J\e[1;1H");

    /* Show numbers */
    const char *avg = (data.averaging == 'F') ? "Fast" : "Slow";
    printf("\nWeighting: %c\nAveraging: %s\n\nL = %.1f dB\nLmax = %.1f dB\nLmin = %.1f dB\n",
           data.weighting, avg, (double)data.spl, (double)data.max_spl, (double)data.min_spl);

    /* Show bar graph */
    const uint8_t max_bar_lvl = 16;
    const char bars[max_bar_lvl + 1] = "----------------";
    const uint8_t bar_lvl = spl::utils::to_bar_level(data.spl, max_bar_lvl);
    printf("\n|%-*.*s|\n0%*s\n\e[u", max_bar_lvl, bar_lvl, bars, max_bar_lvl + 3, "120dB");
}

void console_view::process(void)
{
    /* TODO: Implement console/serial port input command handling */
    if (this->char_received)
    {
        this->char_received = false;

        printf("\e[u%c\e[s", this->received_char);
    }
}
