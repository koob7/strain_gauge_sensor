#include "adc.h"

bool adc_t::init()
{
    state = state_t::READY;
    return true;
}

bool adc_t::wake_up_interface() { return true; }
bool adc_t::configure_interface(uint8_t param_1) { return true; }
int32_t adc_t::read_bridge_voltage() { return 123; }
bool adc_t::sleep_interface() { return true; }