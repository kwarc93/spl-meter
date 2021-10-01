/*
 * hal_microphone.cpp
 *
 *  Created on: 1 paź 2021
 *      Author: kwarc
 */

#include "hal_microphone.hpp"

using namespace hal;

//-----------------------------------------------------------------------------

microphone::microphone(interface::microphone *interface)
{
    this->interface = interface;
}

microphone::~microphone()
{

}

bool microphone::init(hal::interface::microphone::data_ready_callback_t data_ready_callback)
{
    this->interface->init(data_ready_callback);
    return true;
}

void microphone::enable(void)
{
    this->interface->enable();
}

void microphone::disable(void)
{
    this->interface->disable();
}

void microphone::set_gain(float gain)
{
    this->interface->set_gain(gain);
}

int32_t microphone::get_sensitivity(void)
{
    return this->interface->get_sensitivity();
}

uint32_t microphone::get_sampling_frequency(void)
{
    return this->interface->get_sampling_frequency();
}

//-----------------------------------------------------------------------------
