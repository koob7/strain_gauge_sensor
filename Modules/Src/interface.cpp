#include "interface.h"
#include "helper.h"
#include "multiplekser.h"
#include "usart_control.h"
#include <string>

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

    uint32_t start = HAL_GetTick();

    while ((HAL_GetTick() - start) <= MULTIPLEKSER_VOLTAGE_STABLIZATION_TIME)
    {
    };

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

void interface_t::single_measurement(command_t command)
{
    using layout_t = command_t::default_command_layout_t;
    using name_t   = multiplekser_t::multiplekser_name_t;

    bool result = true;

    if (command.parameters[as_int(layout_t::INTERFACE)] >= as_int(interface_t::interface_id_t::INTERFACE_NUMBER))
    {
        g_usart_control->send_frame("Measurement failed, wrong interface\n");
        return;
    }

    result &=
        g_multipleksers[as_int(name_t::BRIDGE_1)].config_output_channel(command.parameters[as_int(layout_t::BRIDGE_1)]);
    result &=
        g_multipleksers[as_int(name_t::BRIDGE_2)].config_output_channel(command.parameters[as_int(layout_t::BRIDGE_2)]);
    result &=
        g_multipleksers[as_int(name_t::BRIDGE_3)].config_output_channel(command.parameters[as_int(layout_t::BRIDGE_3)]);
    result &=
        g_multipleksers[as_int(name_t::BRIDGE_4)].config_output_channel(command.parameters[as_int(layout_t::BRIDGE_4)]);

    if (!result)
    {
        g_usart_control->send_frame("Measurement failed, wrong bridge configuration\n");
        return;
    }

    int32_t differential_voltage = g_interfaces[command.parameters[as_int(layout_t::INTERFACE)]]->meassure(
        command.parameters[as_int(layout_t::EXTRA_PARAM)]);

    if (differential_voltage < 0)
    {
        g_usart_control->send_frame("Measurement probably failed, negative result\n");
    }

    std::string measurement_result_message = "Measurement result: ";
    measurement_result_message += std::to_string(differential_voltage);
    measurement_result_message += " for configuration: interface:";
    measurement_result_message += g_interfaces[command.parameters[as_int(layout_t::INTERFACE)]]->get_char_name();
    measurement_result_message += ", parameters: ";
    for (uint8_t i = as_int(layout_t::BRIDGE_1); i < as_int(layout_t::PARAMETERS_COUNT); i++)
    {
        measurement_result_message += std::to_string(command.parameters[i]) + ", ";
    }
    measurement_result_message += "\n";

    g_usart_control->send_frame(measurement_result_message.c_str());
}