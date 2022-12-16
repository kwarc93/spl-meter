/*
 * lcd_view.hpp
 *
 *  Created on: 3 paź 2021
 *      Author: kwarc
 */

#ifndef VIEW_LCD_VIEW_HPP_
#define VIEW_LCD_VIEW_HPP_

#include "view_interface.hpp"

#include <hal/hal_lcd.hpp>

namespace spl
{
    class lcd_view : public view_interface
    {
    public:
        lcd_view();
        ~lcd_view();

        void update(view_mode view);
        void update(const data &data);
    private:
        void update_lcd(const data *data);
        hal::lcd::lcd_6x14 lcd;
    };
}

#endif /* VIEW_LCD_VIEW_HPP_ */
