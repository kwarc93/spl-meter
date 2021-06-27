/*
 * lcd_gh08172.cpp
 *
 *  Created on: 27 cze 2021
 *      Author: kwarc
 */

#include "lcd_gh08172.hpp"

#include <drivers/stm32l4/gpio.hpp>
#include <drivers/stm32l4/lcd.hpp>

#include <array>

using namespace drivers;

#define LCD_GPIOA_PINS  (GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 |    \
                         GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_15)

#define LCD_GPIOB_PINS  (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 |    \
                         GPIO_PIN_5 | GPIO_PIN_9 | GPIO_PIN_12 |   \
                         GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)

#define LCD_GPIOC_PINS  (GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |    \
                         GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8)

#define LCD_GPIOD_PINS  (GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |   \
                         GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | \
                         GPIO_PIN_14 | GPIO_PIN_15)

lcd_gh08172::lcd_gh08172()
{
    const std::array<const gpio::io, 29> pins =
    {{
        {gpio::port::porta, gpio::pin::pin6},
        {gpio::port::porta, gpio::pin::pin7},
        {gpio::port::porta, gpio::pin::pin8},
        {gpio::port::porta, gpio::pin::pin9},
        {gpio::port::porta, gpio::pin::pin10},
        {gpio::port::porta, gpio::pin::pin15},

        {gpio::port::portb, gpio::pin::pin0},
        {gpio::port::portb, gpio::pin::pin1},
        {gpio::port::portb, gpio::pin::pin4},
        {gpio::port::portb, gpio::pin::pin5},
        {gpio::port::portb, gpio::pin::pin9},
        {gpio::port::portb, gpio::pin::pin12},
        {gpio::port::portb, gpio::pin::pin13},
        {gpio::port::portb, gpio::pin::pin14},
        {gpio::port::portb, gpio::pin::pin15},

        {gpio::port::portc, gpio::pin::pin3},
        {gpio::port::portc, gpio::pin::pin4},
        {gpio::port::portc, gpio::pin::pin5},
        {gpio::port::portc, gpio::pin::pin6},
        {gpio::port::portc, gpio::pin::pin7},
        {gpio::port::portc, gpio::pin::pin8},

        {gpio::port::portd, gpio::pin::pin8},
        {gpio::port::portd, gpio::pin::pin9},
        {gpio::port::portd, gpio::pin::pin10},
        {gpio::port::portd, gpio::pin::pin11},
        {gpio::port::portd, gpio::pin::pin12},
        {gpio::port::portd, gpio::pin::pin13},
        {gpio::port::portd, gpio::pin::pin14},
        {gpio::port::portd, gpio::pin::pin15},
    }};

    for (const auto &pin : pins)
        gpio::init(pin, gpio::af::af11);

    drivers::lcd::init();

    /* Test all segments */
    for (uint8_t idx = 0; idx < 16; idx++)
        *drivers::lcd::ram[idx] = 0xfffffffful;

    drivers::lcd::update();
}

bool lcd_gh08172::write(std::string &s)
{
    return true;
}
