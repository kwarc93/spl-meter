/*
 * hal_lcd.hpp
 *
 *  Created on: 7 lip 2021
 *      Author: kwarc
 */

#ifndef HAL_LCD_HPP_
#define HAL_LCD_HPP_

#include <drivers/stm32l4/gpio.hpp>

#include <array>

namespace hal::lcd
{
    enum com
    {
        COM0 = 0, COM1, COM2, COM3
    };

    /** @brief   Mapping of LCD COM pins to MCU LCD AF pins */
    constexpr std::array<uint8_t, 4> com_map = {0, 1, 2, 3};

    enum segment
    {
        SEG0 = 0, SEG1, SEG2, SEG3, SEG4, SEG5, SEG6, SEG7,
        SEG8, SEG9, SEG10, SEG11, SEG12, SEG13, SEG14, SEG15,
        SEG16, SEG17, SEG18, SEG19, SEG20, SEG21, SEG22, SEG23
    };

    /** @brief   Mapping of LCD SEGx pins to MCDU LCD AF pins */
    constexpr std::array<uint8_t, 24> segment_map =
    {
      4, 23, 6, 13, 15, 29, 31, 33,
      35, 25, 17, 8, 9, 26, 24, 34,
      32, 30, 28, 14, 12, 5, 22, 3
    };

    constexpr std::array<const drivers::gpio::io, 29> gpio =
    {{
        {drivers::gpio::port::porta, drivers::gpio::pin::pin6},
        {drivers::gpio::port::porta, drivers::gpio::pin::pin7},
        {drivers::gpio::port::porta, drivers::gpio::pin::pin8},
        {drivers::gpio::port::porta, drivers::gpio::pin::pin9},
        {drivers::gpio::port::porta, drivers::gpio::pin::pin10},
        {drivers::gpio::port::porta, drivers::gpio::pin::pin15},

        {drivers::gpio::port::portb, drivers::gpio::pin::pin0},
        {drivers::gpio::port::portb, drivers::gpio::pin::pin1},
        {drivers::gpio::port::portb, drivers::gpio::pin::pin4},
        {drivers::gpio::port::portb, drivers::gpio::pin::pin5},
        {drivers::gpio::port::portb, drivers::gpio::pin::pin9},
        {drivers::gpio::port::portb, drivers::gpio::pin::pin12},
        {drivers::gpio::port::portb, drivers::gpio::pin::pin13},
        {drivers::gpio::port::portb, drivers::gpio::pin::pin14},
        {drivers::gpio::port::portb, drivers::gpio::pin::pin15},

        {drivers::gpio::port::portc, drivers::gpio::pin::pin3},   // VLCD
        {drivers::gpio::port::portc, drivers::gpio::pin::pin4},
        {drivers::gpio::port::portc, drivers::gpio::pin::pin5},
        {drivers::gpio::port::portc, drivers::gpio::pin::pin6},
        {drivers::gpio::port::portc, drivers::gpio::pin::pin7},
        {drivers::gpio::port::portc, drivers::gpio::pin::pin8},

        {drivers::gpio::port::portd, drivers::gpio::pin::pin8},
        {drivers::gpio::port::portd, drivers::gpio::pin::pin9},
        {drivers::gpio::port::portd, drivers::gpio::pin::pin10},
        {drivers::gpio::port::portd, drivers::gpio::pin::pin11},
        {drivers::gpio::port::portd, drivers::gpio::pin::pin12},
        {drivers::gpio::port::portd, drivers::gpio::pin::pin13},
        {drivers::gpio::port::portd, drivers::gpio::pin::pin14},
        {drivers::gpio::port::portd, drivers::gpio::pin::pin15},
    }};
}

#endif /* HAL_LCD_HPP_ */
