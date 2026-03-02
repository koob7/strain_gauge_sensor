#include "adc_opamp.h"

bool adc_opamp_t::init()
{
    state = state_t::READY;
    return true;
}

bool adc_opamp_t::wake_up_interface() { return true; }
bool adc_opamp_t::configure_interface(uint8_t param_1) { return true; }
int32_t adc_opamp_t::read_bridge_voltage() { return 123; }
bool adc_opamp_t::sleep_interface() { return true; }