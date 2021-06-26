/*
 * hal_led.hpp
 *
 *  Created on: 25 paź 2020
 *      Author: kwarc
 */

#ifndef HAL_LED_HPP_
#define HAL_LED_HPP_

#include <array>

#include <hal/hal_interface.hpp>

#include <drivers/led_gpio.hpp>

namespace hal
{

//---------------------------------------------------------------------------

    class led
    {
    public:
        led(hal::interface::led *interface);
        virtual ~led();
        void set(uint8_t brightness);
        void set(bool state);
    protected:
        hal::interface::led *interface;
    private:
        uint8_t brightness;
    };

//-----------------------------------------------------------------------------

namespace leds
{
    class debug : public led
    {
    public:
        debug(void) : led {&drv} {}
    private:
        const drivers::gpio::io io = { drivers::gpio::port::porte, drivers::gpio::pin::pin8 };
        drivers::led_gpio drv {io};
    };

    class error : public led
    {
    public:
        error(void) : led {&drv} {}
    private:
        const drivers::gpio::io io = { drivers::gpio::port::portb, drivers::gpio::pin::pin2 };
        drivers::led_gpio drv {io};
    };
}

//---------------------------------------------------------------------------

}

#endif /* HAL_LED_HPP_ */
