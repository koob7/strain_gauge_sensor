#include "adc_module.h"

const char *adc_t::get_char_name() { return "adc"; }
bool adc_t::low_level_wake_up() { return true; }
bool adc_t::low_level_configuration(uint8_t param) { return true; }
bool adc_t::low_level_sleep() { return true; }