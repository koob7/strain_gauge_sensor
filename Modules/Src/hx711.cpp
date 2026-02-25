#include "hx711.h"

bool hx711_t::init()
{
    state = state_t::READY;
    return true;
}