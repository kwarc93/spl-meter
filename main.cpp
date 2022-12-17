/*
 * main.c
 *
 *  Created on: 1 cze 2021
 *      Author: kwarc
 */

#include <cstdio>

#include <hal/hal_system.hpp>
#include <hal/hal_led.hpp>

#include <app/controller/default_controller.hpp>
#include <app/controller/button_user_input.hpp>
#include <app/controller/console_user_input.hpp>

#include <app/view/lcd_view.hpp>
#include <app/view/console_view.hpp>

int main(void)
{
    hal::system::init();

    printf("System started\n");

    auto led = hal::leds::debug();
    bool led_state = true;
    led.set(led_state);

    auto view1 = spl::lcd_view();
    auto view2 = spl::console_view();
    std::vector<spl::view_interface*> views = {&view1, &view2};

    auto usr_input1 = spl::button_user_input();
    auto usr_input2 = spl::console_user_input();
    std::vector<spl::user_input_interface*> usr_inputs = {&usr_input1, &usr_input2};

    auto controller = spl::default_controller(views, usr_inputs);

    auto led_blink_start = hal::system::clock::now();

    while (true)
    {
        controller.process();

        if (hal::system::clock::is_elapsed(led_blink_start, std::chrono::milliseconds(500)))
        {
            led_blink_start = hal::system::clock::now();
            led.set(led_state ^= true);
        }
    }
}
