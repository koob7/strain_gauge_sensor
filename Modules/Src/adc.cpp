#include "adc.h"

bool adc_t::init()
{
    state = state_t::READY;
    return true;
}