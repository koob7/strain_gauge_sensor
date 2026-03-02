#include "hx711.h"

bool hx711_t::init()
{
    state = state_t::READY;
    return true;
}

bool hx711_t::wake_up_interface() { return hx711_driver.power_up(); };

bool hx711_t::configure_interface(uint8_t param_1)
{
    if (param_1 >= as_int(hx711_driver_t::gain_ranges_t::GAIN_RANGES_NUMBER))
    {
        return false;
    }

    return hx711_driver.set_gain(static_cast<hx711_driver_t::gain_ranges_t>(param_1));
};

int32_t hx711_t::read_bridge_voltage() { return static_cast<int32_t>(hx711_driver.read()); };

bool hx711_t::sleep_interface() { return hx711_driver.power_down(); };