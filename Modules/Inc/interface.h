#include "command.h"
#include "main.h"
#include "types.h"

#pragma once

class interface_t
{
  protected:
    static constexpr uint32_t MULTIPLEKSER_VOLTAGE_STABLIZATION_TIME = 100;

  public:
    enum class interface_id_t : uint8_t
    {
        HX711     = 0,
        ADC_OPAMP = 1,
        ADC       = 2,

        // must be last
        INTERFACE_NUMBER,
    };

    interface_id_t interface_id;
    state_t state;
    const uint8_t interface_multiplekser_output;

    interface_t() = delete;
    interface_t(interface_id_t id, uint8_t multiplekser_ouput)
        : interface_id(id), state(state_t::INIT), interface_multiplekser_output(multiplekser_ouput)
    {
        register_interface();
    }

    virtual bool init() = 0;
    virtual int32_t meassure(uint8_t param_1) final;
    virtual const char *get_char_name() = 0;

    state_t const get_state() { return state; }
    interface_id_t const get_interface_id() { return interface_id; }

  private:
    virtual void register_interface() final;

    virtual bool connect_interface() final;
    bool disconnect_multipleksers();
    virtual bool wake_up_interface()                  = 0;
    virtual bool configure_interface(uint8_t param_1) = 0;
    virtual int32_t read_bridge_voltage()             = 0;
    virtual bool sleep_interface()                    = 0;

  public:
    static bool single_measurement(command_t command);
};

extern interface_t *g_interfaces[static_cast<uint8_t>(interface_t::interface_id_t::INTERFACE_NUMBER)];
