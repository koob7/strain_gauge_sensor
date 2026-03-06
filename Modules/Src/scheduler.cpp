#include <stdlib.h>
#include <type_traits>

#include "adc_opamp.h"
#include "d_adc.h"
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

    switch (command.parameters[as_int(layout_t::COMMAND_ID)])
    {

    case as_int(code_t::SCHEDULE_MEASUREMENT):
    {
        if (command.parameters[as_int(layout_t::INTERFACE)] >= as_int(interface_t::interface_id_t::INTERFACE_NUMBER))
        {
            return false;
        }
        scheduled_commands.push_back(command);
        return true;
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
        return true;
    }

    case as_int(code_t::REMOVE_SCHEDULED_MEASUREMENTS):
    {
        scheduled_commands.clear();
        return true;
    }

    case as_int(code_t::EXECUTE_MEASUREMENTS):
    {
        bool result = true;
        for (std::list<command_t>::iterator it = scheduled_commands.begin(); it != scheduled_commands.end(); ++it)
        {
            result |= interface_t::single_measurement(*it);
        }

        return result;
    }

    case (as_int(code_t::PRINT_SCHEDULED_COMMANDS)):
    {
        for (std::list<command_t>::iterator it = scheduled_commands.begin(); it != scheduled_commands.end(); ++it)
        {
            it->print_as_plain_parameters();
        }
        return true;
    }

    case as_int(code_t::SINGLE_MEASUREMENT):
    {
        return interface_t::single_measurement(command);
    }

    case as_int(code_t::SERIALIZE_COMMANDS):
    {
        bool result = true;
        if (command.parameters[1] == 1)
        {
            command_t clear_flash;
            clear_flash.parameters[as_int(layout_t::COMMAND_ID)] = as_int(code_t::ERASE_FLASH);

            result |= g_device_modules[as_int(device_t::module_id_t::SERIALIZER)]->execute_command(clear_flash);
        }

        if (!result)
        {
            return result;
        }

        for (std::list<command_t>::iterator it = scheduled_commands.begin(); it != scheduled_commands.end(); ++it)
        {
            (*it).parameters[as_int(layout_t::COMMAND_ID)] = as_int(code_t::SAVE_COMMAND_TO_FLASH);
            result |= g_device_modules[as_int(device_t::module_id_t::SERIALIZER)]->execute_command(*it);
        }

        return result;
    }

    default:
        return false;
    }

    return false;
}
