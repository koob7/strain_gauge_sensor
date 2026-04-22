#include <stdlib.h>
#include <type_traits>

#include "adc_module.h"
#include "adc_opamp.h"
#include "helper.h"
#include "hx711.h"
#include "multiplekser.h"
#include "scheduler.h"
#include "serializer.h"
#include "temp_module.h"
#include "usart_control.h"

scheduler_t *g_scheduler = nullptr;

bool scheduler_t::init()
{

    static hx711_t hx711(interface_t::interface_id_t::HX711);
    static adc_opamp_t adc_opamp(interface_t::interface_id_t::ADC_OPAMP);
    static adc_t adc(interface_t::interface_id_t::ADC);
    static adc_temp_t adc_temp(interface_t::interface_id_t::TEMP);

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

void scheduler_t::handle()
{
    if (state != state_t::READY)
    {

        bool result = true;

        for (uint8_t i = 0; i < static_cast<uint8_t>(interface_t::interface_id_t::INTERFACE_NUMBER); i++)
        {
            result &= g_interfaces[i]->init();
        }

        if (!result)
        {
            state = state_t::ERROR;
            return;
        }

        state = state_t::READY;
    }

    if (pending_execution)
    {
        pending_execution = false;

        command_t command;
        command.parameters[as_int(command_t::default_command_layout_t::COMMAND_ID)] =
            as_int(command_t::command_code_t::EXECUTE_MEASUREMENTS);

        execute_command(command);
    }
}

bool scheduler_t::execute_command(command_t command, [[maybe_unused]] uint16_t internal_parm)
{
    using code_t   = command_t::command_code_t;
    using layout_t = command_t::default_command_layout_t;

    switch (command.parameters[as_int(layout_t::COMMAND_ID)])
    {

    case as_int(code_t::SCHEDULE_MEASUREMENT):
    {
        g_usart_control->send_frame("Scheduling measurement\n");
        if (command.parameters[as_int(layout_t::INTERFACE)] >= as_int(interface_t::interface_id_t::INTERFACE_NUMBER))
        {
            return false;
        }
        scheduled_commands.push_back(command);
        return true;
    }

    case as_int(code_t::REMOVE_MEASUREMENT):
    {
        g_usart_control->send_frame("Removing measurement with index: %d\n", command.parameters[1]);

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
        g_usart_control->send_frame("Removing all scheduled measurements\n");
        scheduled_commands.clear();
        return true;
    }

    case as_int(code_t::EXECUTE_MEASUREMENTS):
    {
        bool result = true;
        g_usart_control->send_frame("Executing scheduled measurements for reference preassure: %d\n",
                                    command.parameters[1]);
        for (std::list<command_t>::iterator it = scheduled_commands.begin(); it != scheduled_commands.end(); ++it)
        {
            result |= interface_t::single_measurement(*it);
        }

        return result;
    }

    case (as_int(code_t::PRINT_SCHEDULED_COMMANDS)):
    {
        g_usart_control->send_frame("Scheduled commands:\n");
        for (std::list<command_t>::iterator it = scheduled_commands.begin(); it != scheduled_commands.end(); ++it)
        {
            it->print_as_plain_parameters();
        }
        return true;
    }

    case as_int(code_t::SINGLE_MEASUREMENT):
    {
        g_usart_control->send_frame("Executing single measurement\n");
        return interface_t::single_measurement(command);
    }

    case as_int(code_t::SERIALIZE_COMMANDS):
    {
        g_usart_control->send_frame("Serializing scheduled commands\n");

        if (command.parameters[1] >= NUMBER_OF_USER_PAGES)
        {
            return false;
        }

        bool result = true;
        if (command.parameters[2] == 1)
        {
            command_t clear_flash;
            clear_flash.parameters[as_int(layout_t::COMMAND_ID)] = as_int(code_t::ERASE_FLASH);
            clear_flash.parameters[1]                            = command.parameters[1];

            result |= g_device_modules[as_int(device_t::module_id_t::SERIALIZER)]->execute_command(clear_flash);
        }

        if (!result)
        {
            return result;
        }

        for (std::list<command_t>::iterator it = scheduled_commands.begin(); it != scheduled_commands.end(); ++it)
        {
            (*it).parameters[as_int(layout_t::COMMAND_ID)] = as_int(code_t::SAVE_COMMAND_TO_FLASH);
            result |= g_device_modules[as_int(device_t::module_id_t::SERIALIZER)]->execute_command(
                *it, command.parameters[1]);
        }

        return result;
    }

    case as_int(code_t::PERIODIC_MEASUREMENTS):
    {
        if (command.parameters[1] == 0) // disable timer
        {
            pending_execution = false;
            scheduler_timer.stop_timer();
        }
        else if (command.parameters[1] == 1) // enable timer
        {
            uint32_t period_us = command.parameters[2] * SEC_TO_MS * MS_TO_US; // time parsed seconds
            scheduler_timer.set_timer_period(period_us);
            scheduler_timer.start_timer();
        }
        else
        {
            return false;
        }

        return true;
    }

    default:
        return false;
    }

    return false;
}

void scheduler_t::elapsed_callback(void *context)
{
    scheduler_t *scheduler_p       = static_cast<scheduler_t *>(context);
    scheduler_p->pending_execution = true;
}