/*
 * rcc.cpp
 *
 *  Created on: 25 paź 2020
 *      Author: kwarc
 */


#include "rcc.hpp"

#include <cmsis/stm32l4xx.h>

#include <hal/hal_system.hpp>

using namespace drivers;

//-----------------------------------------------------------------------------

static volatile uint32_t *const bus_map[] =
{
    [static_cast<uint8_t>(rcc::bus::AHB1)] = &RCC->AHB1ENR,
    [static_cast<uint8_t>(rcc::bus::AHB2)] = &RCC->AHB2ENR,
    [static_cast<uint8_t>(rcc::bus::AHB3)] = &RCC->AHB3ENR,
    [static_cast<uint8_t>(rcc::bus::APB1)] = &RCC->APB1ENR1,
    [static_cast<uint8_t>(rcc::bus::APB2)] = &RCC->APB2ENR,
};

//-----------------------------------------------------------------------------

void rcc::reset(void)
{
    /* TODO: Complete this function */
}

//-----------------------------------------------------------------------------

void rcc::reset_all_periph(void)
{
    /* TODO: Verify this values */

    RCC->AHB1RSTR = 0xFFFFFFFF;
    RCC->AHB2RSTR = 0xFFFFFFFF;
    RCC->AHB3RSTR = 0xFFFFFFFF;
    RCC->APB1RSTR1 = 0xFFFFFFFF;
    RCC->APB1RSTR2 = 0xFFFFFFFF;
    RCC->APB2RSTR = 0xFFFFFFFF;

    RCC->AHB1RSTR = 0x00000000;
    RCC->AHB2RSTR = 0x00000000;
    RCC->AHB3RSTR = 0x00000000;
    RCC->APB1RSTR1 = 0x00000000;
    RCC->APB1RSTR2 = 0x00000000;
    RCC->APB2RSTR = 0x00000000;
}

//-----------------------------------------------------------------------------

void rcc::disable_all_periph_clocks(void)
{
    /* TODO: Complete this function */
}

//-----------------------------------------------------------------------------

void rcc::enable_periph_clock(const periph_bus &pbus, bool en)
{
    volatile uint32_t *reg = static_cast<volatile uint32_t*>(bus_map[static_cast<uint8_t>(pbus.bus)]);

    if (en)
        *reg |= pbus.periph;
    else
        *reg &= ~(pbus.periph);

    __DSB();
}

//-----------------------------------------------------------------------------

void rcc::set_msi(msi_clock clock)
{
    /* MSIRANGE can be modified when MSI is off (MSION=0) or when MSI is ready */
    RCC->CR &= ~RCC_CR_MSIRANGE_Msk;
    RCC->CR |= static_cast<uint32_t>(clock) << RCC_CR_MSIRANGE_Pos;

    /* The MSIRGSEL bit in RCC->CR selects which MSIRANGE is used.
    If MSIREGSEL is 0, the MSIRANGE in RCC->CSR is used to select the MSI clock.
    If MSIREGSEL is 1, the MSIRANGE in RCC->CR is used. */
    RCC->CR &= ~RCC_CR_MSIRGSEL_Msk;
    RCC->CR |= RCC_CR_MSIRGSEL;

    /* Select MSI as the clock source of System Clock */
    RCC->CR |= RCC_CR_MSION;

    while ((RCC->CR & RCC_CR_MSIRDY) == 0)
    {
        /* Wait for MSI oscillator stabilization. */
    }
}

//-----------------------------------------------------------------------------

void rcc::toggle_hsi(bool state)
{
    if (state)
    {
        RCC->CR |= RCC_CR_HSION;
        while ((RCC->CR & RCC_CR_HSIRDY) == 0)
        {
            /* Wait for HSI oscillator stabilization. */
        }
    }
    else
    {
        RCC->CR &= ~RCC_CR_HSION;
    }
}

void rcc::toggle_hse(bool state)
{
    if (state)
    {
        RCC->CR |= RCC_CR_HSEON;
        while ((RCC->CR & RCC_CR_HSERDY) == 0)
        {
            /* Wait for HSE oscillator stabilization. */
        }
    }
    else
    {
        RCC->CR &= ~RCC_CR_HSEON;
    }
}

//-----------------------------------------------------------------------------

void rcc::toggle_lsi(bool state)
{
    if (state)
    {
        RCC->CSR |= RCC_CSR_LSION;
        while ((RCC->CSR & RCC_CSR_LSIRDY) == 0)
        {
            /* Wait for LSI oscillator stabilization. */
        }
    }
    else
    {
        RCC->CSR &= ~RCC_CSR_LSION;
    }
}

//-----------------------------------------------------------------------------

void rcc::toggle_lse(bool state)
{
    /* Enable access to backup domain. */
    PWR->CR1 |= PWR_CR1_DBP;

    if (state)
    {
        RCC->BDCR |= RCC_BDCR_LSEON;
        while ((RCC->BDCR & RCC_BDCR_LSERDY) == 0)
        {
            /* Wait for LSE oscillator stabilization. */
        }
    }
    else
    {
        RCC->BDCR &= ~RCC_BDCR_LSEON;
    }

    /* Disable access to backup domain. */
    PWR->CR1 &= ~PWR_CR1_DBP;
}

//-----------------------------------------------------------------------------

rcc::reset_source rcc::get_reset_source(void)
{
    return static_cast<rcc::reset_source>(RCC->CSR >> (RCC_CSR_RMVF_Pos + 1));
}

void rcc::clear_reset_source(void)
{
    RCC->CSR |= RCC_CSR_RMVF;
}

//-----------------------------------------------------------------------------
