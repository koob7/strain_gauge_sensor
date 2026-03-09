#include "adc_opamp.h"
#include "helper.h"
#include "multiplekser.h"
#include "opamp.h"

const char *adc_opamp_t::get_char_name() { return "adc_opamp"; }
bool adc_opamp_t::low_level_wake_up()
{
    return HAL_OPAMP_Start(&hopamp1) == HAL_StatusTypeDef::HAL_OK &&
           HAL_OPAMP_Start(&hopamp2) == HAL_StatusTypeDef::HAL_OK;
}
bool adc_opamp_t::low_level_configuration(uint8_t param)
{

    if (param == 0)
    {
        hopamp1.Init.Mode = OPAMP_FOLLOWER_MODE;
        hopamp2.Init.Mode = OPAMP_FOLLOWER_MODE;
        g_multipleksers[as_int(multiplekser_t::multiplekser_name_t::OPAMP_GAIN)].deactivate_multiplekser();
    }
    else
    {
        hopamp1.Init.Mode = OPAMP_STANDALONE_MODE;
        hopamp2.Init.Mode = OPAMP_STANDALONE_MODE;
        g_multipleksers[as_int(multiplekser_t::multiplekser_name_t::OPAMP_GAIN)].config_output_channel(param);
    }
    std::underlying_type_t<HAL_StatusTypeDef> result = as_int(HAL_StatusTypeDef::HAL_OK);

    result |= as_int(HAL_OPAMP_Init(&hopamp1));
    result |= as_int(HAL_OPAMP_Init(&hopamp2));

    uint32_t start = HAL_GetTick();

    while ((HAL_GetTick() - start) <= MULTIPLEKSER_VOLTAGE_STABLIZATION_TIME)
    {
    };

    // HAL_OPAMP_SelfCalibrate(&hopamp1); // If `OPAINTOEN` is enabled, disable it before calling this function
    return result == as_int(HAL_StatusTypeDef::HAL_OK);
}
bool adc_opamp_t::low_level_sleep()
{
    return HAL_OPAMP_Stop(&hopamp1) == HAL_StatusTypeDef::HAL_OK &&
           HAL_OPAMP_Stop(&hopamp2) == HAL_StatusTypeDef::HAL_OK;
}