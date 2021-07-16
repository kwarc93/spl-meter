/*
 * lcd.cpp
 *
 *  Created on: 27 cze 2021
 *      Author: kwarc
 */

#include "lcd.hpp"

#include <cmsis/stm32l4xx.h>

#include "rcc.hpp"
#include "delay.hpp"

using namespace drivers;

volatile uint64_t (&lcd::ram)[8] = reinterpret_cast<volatile uint64_t (&)[8]>(LCD->RAM);

//-----------------------------------------------------------------------------
/* private */

void lcd::wait_for_synchro(void)
{
    while ((LCD->SR & LCD_SR_FCRSR) == 0)
    {
        /* Loop until FCRSF flag is set */
    }
}

//-----------------------------------------------------------------------------
/* public */

bool lcd::init(void)
{
    /* Enable LSE clock & select it as clock source */
    rcc::set_rtc_clock(rcc::rtc_clock_source::LSE);
    rcc::toggle_lse(true);

    /* Enable LCD peripheral Clock */
    rcc::toggle_periph_clock({rcc::bus::APB1, RCC_APB1ENR1_LCDEN}, true);

    if (LCD->CR & LCD_CR_LCDEN)
        return false;

    /* Wait for the external capacitor Cext */
    delay::ms(2);

    /* Configure minimal features */
    LCD->FCR = 0b1111 << LCD_FCR_DIV_Pos | 0b101 << LCD_FCR_CC_Pos | 0b100 << LCD_FCR_PON_Pos |
               0b010 << LCD_FCR_BLINKF_Pos;

    wait_for_synchro();

    LCD->CR = 0b011 << LCD_CR_DUTY_Pos | 0b10 << LCD_CR_BIAS_Pos;

    /* Enable the peripheral */
    LCD->CR |= LCD_CR_LCDEN;

    /* Wait Until the LCD is enabled */
    while((LCD->SR & LCD_SR_ENS) == 0);
    /* Wait Until the LCD Booster is ready */
    while((LCD->SR & LCD_SR_RDY) == 0);

    clear();

    return true;
}

void lcd::deinit(void)
{
    clear();

    LCD->CR = 0;
    LCD->FCR = 0;
    LCD->CLR = 0;
}

void lcd::clear(void)
{
    /* Clear the LCD_RAM registers */
    for (uint8_t idx = 0; idx < 16; idx++)
        LCD->RAM[idx] = 0;

    /* Update the LCD display */
    update();
}

void lcd::update(void)
{
    /* Clear the Update Display Done flag before starting the update display request */
    LCD->CLR |= LCD_CLR_UDDC;

    /* Enable the display request */
    LCD->SR |= LCD_SR_UDR;

    while ((LCD->SR & LCD_SR_UDR) != 0)
    {
        /* Wait Until the LCD is ready */
    }
}
