/*
 * flash.cpp
 *
 *  Created on: 8 gru 2020
 *      Author: kwarc
 */

#include "flash.hpp"

#include <cmsis/stm32l4xx.h>

using namespace drivers;

void flash::set_wait_states(uint32_t sysclk_freq)
{
    /* Calculate wait_states (30M is valid for 2.7V to 3.6V voltage range,
       use 24M for 2.4V to 2.7V, 18M for 2.1V to 2.4V or 16M for  1.8V to 2.1V) */
    uint32_t wait_states = sysclk_freq / 30000000ul;

    /* Trim to max allowed value */
    wait_states &= 0x00000007ul;

    /* Reset caches. */
    FLASH->ACR = FLASH_ACR_DCRST | FLASH_ACR_ICRST;

    /* Enable caches and prefetch */
    FLASH->ACR = FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | wait_states;

    __ISB();
}
