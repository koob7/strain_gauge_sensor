#include "multiplekser.h"
#include "helper.h"

bool multiplekser_t::deactivate_multiplekser()
{
    uint8_t devices_to_deinit = ceil_div(multiplekser_outputs, multiplekser_output_number);
    for (; devices_to_deinit--;)
    {
        if (!config_pin_pointers(multiplekser_output_number * devices_to_deinit))
            return false;

        if (!safe_HALL_pin_set(enable_port, enable_pin, GPIO_PIN_SET))
            return false;
    }
}

bool multiplekser_t::config_output_channel(uint8_t output_channel)
{

    if (!deactivate_multiplekser())
        return false;

    if (!config_pin_pointers(output_channel))
        return false;

    if (!enable_multiplekser_output(output_channel))
        return false;

    return true;
}

bool multiplekser_t::enable_multiplekser_output(uint8_t output_channel)
{
    uint8_t pin_switch = (output_channel - 1) % multiplekser_output_number;
    GPIO_PinState s1_state = pin_switch & 0b0001 ? GPIO_PIN_RESET : GPIO_PIN_SET;
    GPIO_PinState s2_state = pin_switch & 0b0010 ? GPIO_PIN_RESET : GPIO_PIN_SET;
    GPIO_PinState s3_state = pin_switch & 0b0100 ? GPIO_PIN_RESET : GPIO_PIN_SET;

    if (!safe_HALL_pin_set(s1_port, s1_pin, s1_state))
        return false;
    if (!safe_HALL_pin_set(s2_port, s2_pin, s2_state))
        return false;
    if (!safe_HALL_pin_set(s3_port, s3_pin, s3_state))
        return false;

    if (!safe_HALL_pin_set(enable_port, enable_pin, GPIO_PIN_RESET))
        return false;

    return true;
}

bool multiplekser_t::safe_HALL_pin_set(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    if (GPIOx != nullptr)
    {
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, PinState);
        return true;
    }
    return false;
}

bool multiplekser_t::config_pin_pointers(uint8_t output_channel)
{
    if (output_channel > multiplekser_outputs)
    {
        return false;
    }

    switch (multiplekser_type)
    {

    case multiplekser_name_t::BRIDGE_1:
        if (output_channel <= multiplekser_output_number)
        {
            // s1_port = ;
            // s1_pin = ;
            // s2_port = ;
            // s2_pin = ;
            // s3_port = ;
            // s3_pin = ;
            // enable_port = ;
            // enable_pin = ;
        }
        else
        {
            // s1_port = ;
            // s1_pin = ;
            // s2_port = ;
            // s2_pin = ;
            // s3_port = ;
            // s3_pin = ;
            // enable_port = ;
            // enable_pin = ;
        }
        break;

    case multiplekser_name_t::BRIDGE_2:
        // s1_port = ;
        // s1_pin = ;
        // s2_port = ;
        // s2_pin = ;
        // s3_port = ;
        // s3_pin = ;
        // enable_port = ;
        // enable_pin = ;
        break;

    default:
        s1_port = nullptr;
        s1_pin = 0;
        s2_port = nullptr;
        s2_pin = 0;
        s3_port = nullptr;
        s3_pin = 0;
        enable_port = nullptr;
        enable_pin = 0;
        return false;
    };
    return true;
}