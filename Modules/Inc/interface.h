#include "main.h"
#include "types.h"

#pragma once

class interface_t
{
  public:
    enum class interface_id_t : uint8_t
    {
        HX711,
        ADC_OPAMP,
        ADC,

        // must be last
        INTERFACE_NUMBER,
    };

    interface_id_t interface_id;
    state_t state;

    interface_t() = delete;
    interface_t(interface_id_t id) : interface_id(id), state(state_t::INIT) { register_interface(); }

    virtual bool init() = 0;
    virtual int32_t meassure(uint8_t param_1) = 0;
    virtual const char *get_char_name() = 0;

    void register_interface();

    state_t const get_state() { return state; }
    interface_id_t const get_interface_id() { return interface_id; }
};

extern interface_t *g_interfaces[static_cast<uint8_t>(interface_t::interface_id_t::INTERFACE_NUMBER)];
