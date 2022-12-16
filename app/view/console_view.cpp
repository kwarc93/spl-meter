/*
 * console_view.cpp
 *
 *  Created on: 16 gru 2022
 *      Author: kwarc
 */

#include "console_view.hpp"

#include <cstdio>

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
 * 0            120
 */

//-----------------------------------------------------------------------------
/* private */

//-----------------------------------------------------------------------------
/* public */

console_view::console_view()
{

}

console_view::~console_view()
{

}

void console_view::update(view_mode view)
{
    /* Do nothing, because all modes are displayed together */
}

void console_view::update(const data &data)
{
    /* Clear screen */
    printf("\e[2J");

    /* Show numbers */
    const char *avg = (data.averaging == 'F') ? "Fast" : "Slow";
    printf("\nWeighting: %c\nAveraging: %s\n\nL = %.1f db\nLmax = %.1f db\nLmin = %.1f db\n",
           data.weighting, avg, (double)data.spl, (double)data.max_spl, (double)data.min_spl);

    /* Show bar graph */
}
