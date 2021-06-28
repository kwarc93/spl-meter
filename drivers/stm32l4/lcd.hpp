/*
 * lcd.hpp
 *
 *  Created on: 26 cze 2021
 *      Author: kwarc
 */

#ifndef STM32L4_LCD_HPP_
#define STM32L4_LCD_HPP_

#include <stdint.h>

namespace drivers
{

class lcd final
{
public:
    lcd() = delete;

    static bool init(void);
    static void deinit(void);
    static void clear(void);
    static void update(void);
    static volatile uint32_t (&ram)[16];

private:
    static void wait_for_synchro(void);
};

}

#endif /* STM32L4_LCD_HPP_ */
