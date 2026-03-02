#include "multiplekser.h"
#include "helper.h"

multiplekser_t g_multipleksers[] = {multiplekser_t(multiplekser_t::multiplekser_name_t::BRIDGE_1),
                                    multiplekser_t(multiplekser_t::multiplekser_name_t::BRIDGE_2),
                                    multiplekser_t(multiplekser_t::multiplekser_name_t::BRIDGE_3),
                                    multiplekser_t(multiplekser_t::multiplekser_name_t::BRIDGE_4),
                                    multiplekser_t(multiplekser_t::multiplekser_name_t::MEASSURING_SOURCE_N),
                                    multiplekser_t(multiplekser_t::multiplekser_name_t::MEASSURING_SOURCE_P),
                                    multiplekser_t(multiplekser_t::multiplekser_name_t::OPAMP_GAIN)};

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

/**
 * @brief function to switch between multiplekser outputs
 *
 * @param output_channel indeksable from 1 to 8 in single multiplekser configuration (two multiplekser can be assigned
 * to one enum so we can choose between 1-16 etc...)
 * @return true
 * @return false
 */
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
    uint8_t pin_switch     = (output_channel - 1) % multiplekser_output_number;
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
        // left upper
        if (output_channel <= multiplekser_output_number)
        {
            // IC8
            s1_port     = MULTI7_S1_GPIO_Port;
            s1_pin      = MULTI7_S3_Pin;
            s2_port     = MULTI7_S2_GPIO_Port;
            s2_pin      = MULTI7_S2_Pin;
            s3_port     = MULTI7_S3_GPIO_Port;
            s3_pin      = MULTI7_S3_Pin;
            enable_port = MULTI7_E_GPIO_Port;
            enable_pin  = MULTI7_E_Pin;
        }
        else
        {
            // IC7
            s1_port     = MULTI6_S1_GPIO_Port;
            s1_pin      = MULTI6_S3_Pin;
            s2_port     = MULTI6_S2_GPIO_Port;
            s2_pin      = MULTI6_S2_Pin;
            s3_port     = MULTI6_S3_GPIO_Port;
            s3_pin      = MULTI6_S3_Pin;
            enable_port = MULTI6_E_GPIO_Port;
            enable_pin  = MULTI6_E_Pin;
        }
        break;

    case multiplekser_name_t::BRIDGE_2:
        // left lower

        //  IC9
        s1_port     = MULTI8_S1_GPIO_Port;
        s1_pin      = MULTI8_S3_Pin;
        s2_port     = MULTI8_S2_GPIO_Port;
        s2_pin      = MULTI8_S2_Pin;
        s3_port     = MULTI8_S3_GPIO_Port;
        s3_pin      = MULTI8_S3_Pin;
        enable_port = MULTI8_E_GPIO_Port;
        enable_pin  = MULTI8_E_Pin;
        break;

    case multiplekser_name_t::BRIDGE_3:
        // right upper

        //  IC3
        s1_port     = MULTI4_S1_GPIO_Port;
        s1_pin      = MULTI4_S3_Pin;
        s2_port     = MULTI4_S2_GPIO_Port;
        s2_pin      = MULTI4_S2_Pin;
        s3_port     = MULTI4_S3_GPIO_Port;
        s3_pin      = MULTI4_S3_Pin;
        enable_port = MULTI4_E_GPIO_Port;
        enable_pin  = MULTI4_E_Pin;
        break;

    case multiplekser_name_t::BRIDGE_4:
        // right lower

        // IC4
        s1_port     = MULTI5_S1_GPIO_Port;
        s1_pin      = MULTI5_S3_Pin;
        s2_port     = MULTI5_S2_GPIO_Port;
        s2_pin      = MULTI5_S2_Pin;
        s3_port     = MULTI5_S3_GPIO_Port;
        s3_pin      = MULTI5_S3_Pin;
        enable_port = MULTI5_E_GPIO_Port;
        enable_pin  = MULTI5_E_Pin;
        break;

    case multiplekser_name_t::MEASSURING_SOURCE_P:
        // IC5
        s1_port     = MULTI2_S1_GPIO_Port;
        s1_pin      = MULTI2_S3_Pin;
        s2_port     = MULTI2_S2_GPIO_Port;
        s2_pin      = MULTI2_S2_Pin;
        s3_port     = MULTI2_S3_GPIO_Port;
        s3_pin      = MULTI2_S3_Pin;
        enable_port = MULTI2_E_GPIO_Port;
        enable_pin  = MULTI2_E_Pin;
        break;

    case multiplekser_name_t::MEASSURING_SOURCE_N:
        // IC6
        s1_port     = MULTI3_S1_GPIO_Port;
        s1_pin      = MULTI3_S3_Pin;
        s2_port     = MULTI3_S2_GPIO_Port;
        s2_pin      = MULTI3_S2_Pin;
        s3_port     = MULTI3_S3_GPIO_Port;
        s3_pin      = MULTI3_S3_Pin;
        enable_port = MULTI3_E_GPIO_Port;
        enable_pin  = MULTI3_E_Pin;
        break;

    case multiplekser_name_t::OPAMP_GAIN:
        // IC1
        s1_port     = MULTI1_S1_GPIO_Port;
        s1_pin      = MULTI1_S3_Pin;
        s2_port     = MULTI1_S2_GPIO_Port;
        s2_pin      = MULTI1_S2_Pin;
        s3_port     = MULTI1_S3_GPIO_Port;
        s3_pin      = MULTI1_S3_Pin;
        enable_port = MULTI1_E_GPIO_Port;
        enable_pin  = MULTI1_E_Pin;
        break;

    default:
        s1_port     = nullptr;
        s1_pin      = 0;
        s2_port     = nullptr;
        s2_pin      = 0;
        s3_port     = nullptr;
        s3_pin      = 0;
        enable_port = nullptr;
        enable_pin  = 0;
        return false;
    };
    return true;
}