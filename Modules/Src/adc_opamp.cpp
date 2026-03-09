#include "adc_opamp.h"

const char *adc_opamp_t::get_char_name() { return "adc_opamp"; }
bool adc_opamp_t::low_level_wake_up() { return true; }
bool adc_opamp_t::low_level_configuration(uint8_t param) { return true; }
bool adc_opamp_t::low_level_sleep() { return true; }