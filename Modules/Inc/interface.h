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
    interface_t(interface_id_t id) : interface_id(id), state(state_t::INIT) {}

    virtual bool init() = 0;
    virtual int32_t meassure(uint8_t param_1) = 0;
    virtual const char *get_char_name() = 0;

    void register_interface();

    state_t const get_state() { return state; }
    interface_id_t const get_interface_id() { return interface_id; }
};

extern interface_t *g_interfaces[static_cast<uint8_t>(interface_t::interface_id_t::INTERFACE_NUMBER)];

class adc_t : public interface_t
{
  public:
    adc_t(interface_id_t id) : interface_t(id) {}
    bool init()
    {
        state = state_t::READY;
        return true;
    }
    int32_t meassure(uint8_t param_1) { return 0; }
    const char *get_char_name() { return "adc"; }
};

class hx711_t : public interface_t
{
  public:
    hx711_t(interface_id_t id) : interface_t(id) {}
    bool init()
    {
        state = state_t::READY;
        return true;
    }
    int32_t meassure(uint8_t param_1) { return 0; }
    const char *get_char_name() { return "hx711"; }
};

class adc_opamp_t : public interface_t
{
  public:
    adc_opamp_t(interface_id_t id) : interface_t(id) {}
    bool init()
    {
        state = state_t::READY;
        return true;
    }
    int32_t meassure(uint8_t param_1) { return 0; }
    const char *get_char_name() { return "adc_opamp"; }
};