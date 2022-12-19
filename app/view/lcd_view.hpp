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
#include <hal/hal_button.hpp>

namespace spl
{
    class lcd_view : public view_interface
    {
    public:
        lcd_view();
        ~lcd_view();

        void update(const data &data) override;
        void process(void) override;
    private:
        enum class view_mode
        {
            spl, max, min, all
        };

        void update_lcd(const data *data);
        void update_view_mode(view_mode view);
        void send_event(const event_t &e);

        view_mode current_view_mode;

        hal::lcd::lcd_6x14 lcd;
        hal::buttons::center_btn center_btn;
        hal::buttons::up_btn up_btn;
        hal::buttons::down_btn down_btn;
        hal::buttons::left_btn left_btn;
        hal::buttons::right_btn right_btn;
    };
}

#endif /* VIEW_LCD_VIEW_HPP_ */
