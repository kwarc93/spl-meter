/*
 * hal_interface.hpp
 *
 *  Created on: 21 paź 2020
 *      Author: kwarc
 */

#ifndef HAL_INTERFACE_HPP_
#define HAL_INTERFACE_HPP_

#include <cstddef>
#include <cstdint>

namespace hal::interface
{
    class serial
    {
    public:
        virtual ~serial() {};
        virtual std::byte read(void) = 0;
        virtual void write(std::byte byte) = 0;
        virtual std::size_t read(std::byte *data, std::size_t size) = 0;
        virtual std::size_t write(const std::byte *data, std::size_t size) = 0;
    };

    class temperature_sensor
    {
    public:
        virtual ~temperature_sensor() {};
        virtual float read_temperature(void) = 0;
    };

    class led
    {
    public:
        virtual ~led() {};
        virtual void set(uint8_t brightness) = 0;
    };

    class microphone
    {
    public:
        typedef void (*data_ready_callback_t)(const int16_t *data, uint16_t data_len);

        virtual ~microphone() {};
        virtual void init(data_ready_callback_t data_ready) = 0;
        virtual void enable(void) = 0;
        virtual void disable(void) = 0;
        virtual void set_gain(float gain) = 0;
        virtual uint32_t get_sampling_frequency(void) = 0;
    };
}

#endif /* HAL_INTERFACE_HPP_ */
