/*
 * hal.cpp
 *
 *  Created on: 20 paź 2020
 *      Author: kwarc
 */

#include "hal_system.hpp"

#include <cassert>

#include <hal/hal_usart.hpp>

#include <drivers/stm32l4/core.hpp>
#include <drivers/stm32l4/rcc.hpp>
#include <drivers/stm32l4/flash.hpp>

using namespace hal;

//-----------------------------------------------------------------------------

volatile uint32_t system::systick = 0;

//-----------------------------------------------------------------------------

void system::init(void)
{
    /* Number of group priorities: 16, subpriorities: 16. */
    NVIC_SetPriorityGrouping(0x07 - __NVIC_PRIO_BITS);

    drivers::core::enable_cycles_counter();
    drivers::flash::set_wait_states(system::system_clock);

//    const drivers::rcc::bus_presc presc_cfg =
//    {
//        RCC_CFGR_HPRE_DIV1,
//        RCC_CFGR_PPRE1_DIV1,
//        RCC_CFGR_PPRE2_DIV1,
//    };
//
//    const drivers::rcc::main_pll pll_cfg =
//    {
//        RCC_PLLCFGR_PLLSRC_HSE,
//        1,
//        12,
//        2
//    };
//
//    drivers::rcc::set_main_pll(pll_cfg, presc_cfg);

    drivers::rcc::set_msi(drivers::rcc::msi_clock::CLK_48MHZ);

    drivers::rcc::toggle_periph_clock({drivers::rcc::bus::APB1, RCC_APB1ENR1_PWREN}, true);

    /* Set System Tick interrupt */
    SysTick_Config(system::system_clock / system::systick_freq);
}

//-----------------------------------------------------------------------------
/* syscalls */

extern "C" int _write (int fd, char *ptr, int len)
{
    auto &debug = usart::debug::get_instance();
    return debug.write(reinterpret_cast<std::byte*>(ptr), len);
}

extern "C" int _read (int fd, char *ptr, int len)
{
    auto &debug = usart::debug::get_instance();
    return debug.read(reinterpret_cast<std::byte*>(ptr), len);
}

extern "C" void _ttywrch(int ch)
{
    auto &debug = usart::debug::get_instance();
    debug.write(static_cast<std::byte>(ch));
}
