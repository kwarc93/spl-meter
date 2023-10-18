/*
 * console_view.cpp
 *
 *  Created on: 16 gru 2022
 *      Author: kwarc
 */

#include "console_view.hpp"

#include <cmath>
#include <cstdio>

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

#define CLI_START   "\e[12;1H> "
#define GUI_END     "\e[10;21H"

//-----------------------------------------------------------------------------
/* private */

void console_view::character_received_callback(const std::byte *data, std::size_t bytes_read)
{
    /* WARNING: This is called from interrupt */

    if (bytes_read == 0)
        return;

    while (bytes_read--)
        this->char_queue.push(static_cast<char>(*(data++)));
}

//-----------------------------------------------------------------------------
/* public */

console_view::console_view() : stdio_serial { hal::usart::stdio::get_instance() }
{
    /* Clear screen */
    printf("\e[2J");

    /* Start listening for character */
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
    this->current_data = data;

    /* Clear screen & reset cursor at the beginning */
    printf(GUI_END "\e[1J\e[1;1H");

    /* Show numbers */
    const char *avg = (data.averaging == 'F') ? "Fast" : "Slow";
    printf("\nWeighting: %c\nAveraging: %s\n\nL = %.1f dB\nLmax = %.1f dB\nLmin = %.1f dB\n",
           data.weighting, avg, (double)data.spl, (double)data.max_spl, (double)data.min_spl);

    /* Show bar graph */
    const uint8_t max_bar_lvl = 16;
    const char bars[max_bar_lvl + 1] = "----------------";
    const uint8_t bar_lvl = spl::utils::to_bar_level(data.spl, max_bar_lvl);
    printf("\n|%-*.*s|\n0%*s\n", max_bar_lvl, bar_lvl, bars, max_bar_lvl + 3, "120dB");
}

void console_view::process(void)
{
    /* TODO: Implement a proper CLI shell  */
    while (!this->char_queue.empty())
    {
        char c;
        if (!this->char_queue.pop(c))
            continue;

        switch (c)
        {
            case 'w':
            {
                /* Weighting */
                change_weighting_evt_t e;
                e.weighting = utils::switch_weighting(this->current_data.weighting);
                this->notify(e);
                break;
            }
            case 'a':
            {
                /* Averaging */
                change_averaging_evt_t e;
                if (this->current_data.averaging == 'S')
                    e.averaging = spl::averaging_t::fast;
                else if (this->current_data.averaging == 'F')
                    e.averaging = spl::averaging_t::slow;

                this->notify(e);
                break;
            }
            case 's':
            {
                /* Statistics (clear MIN/MAX) */
                clear_max_spl_data_evt_t e1;
                clear_min_spl_data_evt_t e2;
                this->notify(e1);
                this->notify(e2);
                break;
            }
            case 'e':
            {
                printf(CLI_START "Error code: %d        ", this->current_data.error_code);
                break;
            }
            default:
                printf(CLI_START "Command '%c' not found", c);
        }
    }
}
