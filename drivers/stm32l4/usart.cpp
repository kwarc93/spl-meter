/*
 * usart.cpp
 *
 *  Created on: 20 paź 2020
 *      Author: kwarc
 */

#include "usart.hpp"

#include <map>

#include <cmsis/stm32l4xx.h>

#include <hal/hal_system.hpp>

#include <drivers/stm32l4/gpio.hpp>
#include <drivers/stm32l4/rcc.hpp>

using namespace drivers;

struct usart::usart_hw
{
    USART_TypeDef *const reg;
    rcc::periph_bus pbus;

    gpio::af pin_af;
    gpio::io tx_pin;
    gpio::io rx_pin;
};

static const std::map<usart::id, usart::usart_hw> usartx =
{
    {usart::id::usart2,
    {USART2, { rcc::bus::APB1, RCC_APB1ENR1_USART2EN }, gpio::af::af7,
    { gpio::port::portd, gpio::pin::pin5 }, { gpio::port::portd, gpio::pin::pin6 }}},
};

usart::usart(id id, uint32_t baudrate) : hw (usartx.at(id))
{
    rcc::toggle_periph_clock(this->hw.pbus, true);

    gpio::configure(this->hw.tx_pin, gpio::mode::af, this->hw.pin_af);
    gpio::configure(this->hw.rx_pin, gpio::mode::af, this->hw.pin_af);

    this->hw.reg->BRR = (uint32_t) (hal::system::system_clock + baudrate / 2) / baudrate;
    this->hw.reg->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

std::byte usart::read()
{
    while (!( this->hw.reg->ISR & USART_ISR_RXNE));
    return static_cast<std::byte>(this->hw.reg->RDR);
}

void usart::write(std::byte byte)
{
    while (!( this->hw.reg->ISR & USART_ISR_TXE));
    this->hw.reg->TDR = std::to_integer<volatile uint32_t>(byte);
}

std::size_t usart::read(std::byte *data, std::size_t size)
{
    std::size_t bytes_read = 0;

    while (size--)
    {
        *data++ = this->read();
        bytes_read++;
    }

    return bytes_read;
}

std::size_t usart::write(const std::byte *data, std::size_t size)
{
    std::size_t bytes_written = 0;

    while (size--)
    {
        this->write(*data++);
        bytes_written++;
    }

    return bytes_written;
}

