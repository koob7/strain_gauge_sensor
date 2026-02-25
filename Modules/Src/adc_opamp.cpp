#include "adc_opamp.h"

bool adc_opamp_t::init()
{
    state = state_t::READY;
    return true;
}