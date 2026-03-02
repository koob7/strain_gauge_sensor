#include "interface.h"
#include "helper.h"
#include "multiplekser.h"
#include "usart_control.h"

interface_t *g_interfaces[static_cast<uint8_t>(interface_t::interface_id_t::INTERFACE_NUMBER)] = {};

void interface_t::register_interface()
{
    if (g_interfaces[static_cast<uint8_t>(interface_id)] != nullptr)
    {
        g_usart_control->send_frame("ERR: second interface initiated\n");
        Error_Handler();
    }
    g_interfaces[static_cast<uint8_t>(interface_id)] = this;
}

int32_t interface_t::meassure(uint8_t param_1)
{
    bool result                  = true;
    int32_t differential_voltage = -1;

    result &= connect_interface();
    result &= wake_up_interface();

    if (!result)
        return -1;

    result &= configure_interface(param_1);

    if (!result)
        return -1;

    differential_voltage = read_bridge_voltage();
    result &= sleep_interface();

    if (!result)
        return -1;

    return differential_voltage;
}

bool interface_t::connect_interface()
{
    bool result = true;

    result &= g_multipleksers[as_int(multiplekser_t::multiplekser_name_t::MEASSURING_SOURCE_N)].config_output_channel(
        interface_multiplekser_output);
    result &= g_multipleksers[as_int(multiplekser_t::multiplekser_name_t::MEASSURING_SOURCE_P)].config_output_channel(
        interface_multiplekser_output);

    return result;
}