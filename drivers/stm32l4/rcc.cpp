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
/* private */

static volatile uint32_t *const bus_map[] =
{
    [static_cast<uint8_t>(rcc::bus::AHB1)] = &RCC->AHB1ENR,
    [static_cast<uint8_t>(rcc::bus::AHB2)] = &RCC->AHB2ENR,
    [static_cast<uint8_t>(rcc::bus::AHB3)] = &RCC->AHB3ENR,
    [static_cast<uint8_t>(rcc::bus::APB1)] = &RCC->APB1ENR1,
    [static_cast<uint8_t>(rcc::bus::APB2)] = &RCC->APB2ENR,
};

static const uint8_t ahb_presc_table[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
static const uint8_t apb_presc_table[8]  = {0, 0, 0, 0, 1, 2, 3, 4};

//-----------------------------------------------------------------------------

uint32_t rcc::get_ahb_freq(void)
{
    return (get_sysclk_freq() >> ahb_presc_table[(RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos]);
}

//-----------------------------------------------------------------------------

uint32_t rcc::get_apb1_freq(void)
{
    return (get_sysclk_freq() >> apb_presc_table[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos]);
}

//-----------------------------------------------------------------------------

uint32_t rcc::get_apb2_freq(void)
{
    return (get_sysclk_freq() >> apb_presc_table[(RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos]);
}

//-----------------------------------------------------------------------------

uint8_t rcc::get_ahb_presc(void)
{
    return (1 << ahb_presc_table[(RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos]);
}

//-----------------------------------------------------------------------------

uint8_t rcc::get_apb1_presc(void)
{
    return (1 << apb_presc_table[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos]);
}

//-----------------------------------------------------------------------------

uint8_t rcc::get_apb2_presc(void)
{
    return (1 << apb_presc_table[(RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos]);
}

//-----------------------------------------------------------------------------

uint32_t rcc::get_sysclk_from_pll_source(void)
{
    uint32_t pllvco;
    uint32_t pllm;
    uint32_t pllp;

    /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLLM) * PLLN, SYSCLK = PLL_VCO / PLLP */
    pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
    if (((uint32_t) (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC)) != RCC_PLLCFGR_PLLSRC_HSI)
    {
        /* HSE used as PLL clock source */
        pllvco = (uint32_t) ((((uint64_t) hal::system::hse_clock
                * ((uint64_t) ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos)))) / (uint64_t) pllm);
    }
    else
    {
        /* HSI used as PLL clock source */
        pllvco = (uint32_t) ((((uint64_t) hal::system::hsi_clock
                * ((uint64_t) ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos)))) / (uint64_t) pllm);
    }

    pllp = ((((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >> RCC_PLLCFGR_PLLP_Pos) + 1U) * 2U);

    return pllvco / pllp;
}

//-----------------------------------------------------------------------------
/* public */

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

void rcc::toggle_periph_clock(const periph_bus &pbus, bool state)
{
    volatile uint32_t *reg = static_cast<volatile uint32_t*>(bus_map[static_cast<uint8_t>(pbus.bus)]);

    if (state)
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

uint32_t rcc::get_sysclk_freq(void)
{
    uint32_t sysclockfreq = 0U;

    /* Get SYSCLK source */
    switch (RCC->CFGR & RCC_CFGR_SWS)
    {
    case RCC_CFGR_SWS_MSI:
        sysclockfreq = hal::system::msi_clock;
        break;
    case RCC_CFGR_SWS_HSI: /* HSI used as system clock source */
        sysclockfreq = hal::system::hsi_clock;
        break;

    case RCC_CFGR_SWS_HSE: /* HSE used as system clock source */
        sysclockfreq = hal::system::hse_clock;
        break;

    case RCC_CFGR_SWS_PLL: /* PLL used as system clock source */
        sysclockfreq = get_sysclk_from_pll_source();
        break;

    default:
        sysclockfreq = hal::system::hsi_clock;
        break;
    }
    return sysclockfreq;
}

//-----------------------------------------------------------------------------

uint32_t rcc::get_bus_freq(rcc::bus bus)
{
    switch (bus)
    {
    case rcc::bus::AHB1:
    case rcc::bus::AHB2:
    case rcc::bus::AHB3:
        return get_ahb_freq();
        break;
    case rcc::bus::APB1:
        return get_apb1_freq();
        break;
    case rcc::bus::APB2:
        return get_apb2_freq();
        break;
    default:
        return 0;
    }
}

//-----------------------------------------------------------------------------

int8_t rcc::get_bus_presc(rcc::bus bus)
{
    switch (bus)
    {
    case rcc::bus::AHB1:
    case rcc::bus::AHB2:
    case rcc::bus::AHB3:
        return get_ahb_presc();
        break;
    case rcc::bus::APB1:
        return get_apb1_presc();
        break;
    case rcc::bus::APB2:
        return get_apb2_presc();
        break;
    default:
        return -1;
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

void rcc::set_rtc_clock(rtc_clock_source source)
{
    /* Enable access to backup domain. */
    PWR->CR1 |= PWR_CR1_DBP;

    /* Reset Backup Domain */
    RCC->BDCR |= RCC_BDCR_BDRST;
    RCC->BDCR &= ~RCC_BDCR_BDRST;

    RCC->BDCR &= ~RCC_BDCR_RTCSEL_Msk;
    RCC->BDCR |= static_cast<uint32_t>(source) << RCC_BDCR_RTCSEL_Pos;

    if (source != rcc::rtc_clock_source::NONE)
        RCC->BDCR |= RCC_BDCR_RTCEN;
    else
        RCC->BDCR &= ~RCC_BDCR_RTCEN;

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
