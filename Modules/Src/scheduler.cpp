#include <stdlib.h>
#include <string>
#include <type_traits>

#include "adc.h"
#include "adc_opamp.h"
#include "helper.h"
#include "hx711.h"
#include "multiplekser.h"
#include "scheduler.h"
#include "usart_control.h"

scheduler_t *g_scheduler = nullptr;

bool scheduler_t::init()
{

    static hx711_t hx711(interface_t::interface_id_t::HX711);
    static adc_opamp_t adc_opamp(interface_t::interface_id_t::ADC_OPAMP);
    static adc_t adc(interface_t::interface_id_t::ADC);

    bool result = true;

    for (uint8_t i = 0; i < static_cast<uint8_t>(interface_t::interface_id_t::INTERFACE_NUMBER); i++)
    {
        result &= g_interfaces[i]->init();
    }

    if (!result)
    {
        state = state_t::ERROR;
        return false;
    }

    state = state_t::READY;

    g_usart_control->send_frame("command module initiated");
    return true;
}

bool scheduler_t::execute_command(command_t command)
{
    using code_t   = command_t::command_code_t;
    using layout_t = command_t::default_command_layout_t;
    using name_t   = multiplekser_t::multiplekser_name_t;

    switch (command.parameters[as_int(layout_t::COMMAND_ID)])
    {

    case as_int(code_t::SCHEDULE_MEASUREMENT):
    {
        if (command.parameters[as_int(layout_t::INTERFACE)] >= as_int(interface_t::interface_id_t::INTERFACE_NUMBER))
        {
            return false;
        }
        scheduled_commands.push_back(command);
        break;
    }

    case as_int(code_t::REMOVE_MEASUREMENT):
    {
        // here we do not use default layout so we have to manualy move between table blocks
        if (command.parameters[1] >= scheduled_commands.size())
        {
            return false;
        }

        std::list<command_t>::iterator element_0 = scheduled_commands.begin();
        std::advance(element_0, command.parameters[1]);

        scheduled_commands.erase(element_0);
        break;
    }

    case as_int(code_t::REMOVE_ALL_MEASUREMENTS):
    {
        scheduled_commands.clear();
        break;
    }

    case as_int(code_t::EXECUTE_MEASUREMENTS):
    {

        for (std::list<command_t>::iterator it = scheduled_commands.begin(); it != scheduled_commands.end(); ++it)
        {
            bool result = true;

            if (it->parameters[as_int(layout_t::INTERFACE)] >= as_int(interface_t::interface_id_t::INTERFACE_NUMBER))
            {
                g_usart_control->send_frame("Measurement failed, wrong interface\n");
                continue;
            }

            result &= g_multipleksers[as_int(name_t::BRIDGE_1)].config_output_channel(
                it->parameters[as_int(layout_t::BRIDGE_1)]);
            result &= g_multipleksers[as_int(name_t::BRIDGE_2)].config_output_channel(
                it->parameters[as_int(layout_t::BRIDGE_2)]);
            result &= g_multipleksers[as_int(name_t::BRIDGE_3)].config_output_channel(
                it->parameters[as_int(layout_t::BRIDGE_3)]);
            result &= g_multipleksers[as_int(name_t::BRIDGE_4)].config_output_channel(
                it->parameters[as_int(layout_t::BRIDGE_4)]);

            if (!result)
            {
                g_usart_control->send_frame("Measurement failed, wrong bridge configuration\n");
            }

            int32_t differential_voltage = g_interfaces[it->parameters[as_int(layout_t::INTERFACE)]]->meassure(
                it->parameters[as_int(layout_t::EXTRA_PARAM)]);

            if (differential_voltage < 0)
            {
                g_usart_control->send_frame("Measurement failed, error: %d during interface measurement\n",
                                            differential_voltage);
            }

            std::string measurement_result_message = "Measurement result: ";
            measurement_result_message += std::to_string(differential_voltage);
            measurement_result_message += " for configuration: interface:";
            measurement_result_message += g_interfaces[it->parameters[as_int(layout_t::INTERFACE)]]->get_char_name();
            measurement_result_message += ", parameters: ";
            for (uint8_t i = as_int(layout_t::BRIDGE_1); i < as_int(layout_t::PARAMETERS_COUNT); i++)
            {
                measurement_result_message += std::to_string(it->parameters[i]) + ", ";
            }
            measurement_result_message += "\n";

            g_usart_control->send_frame(measurement_result_message.c_str());
        }

        break;
    }

    case as_int(code_t::SERIALIZE_COMMANDS):
    {
        break;
    }

    case as_int(code_t::RESTORE_SERIALIZED_COMMANDS):
    {
        break;
    }

    case as_int(code_t::REMOVE_SERIALIZED_COMMANDS):
    {
        break;
    }

    default:
        return false;
    }

    return true;
}