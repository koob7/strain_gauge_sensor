#include "adc_interface.h"
#include "helper.h"

bool adc_interface_t::init()
{
    state = state_t::READY;

    return true;
}

bool adc_interface_t::wake_up_interface()
{
    std::underlying_type_t<HAL_StatusTypeDef> status = as_int(HAL_StatusTypeDef::HAL_OK);

    status |= as_int(ADC_Enable(&hadc));
    status |= !low_level_wake_up();
    return status == as_int(HAL_StatusTypeDef::HAL_OK);
}
bool adc_interface_t::configure_interface(uint8_t param_1)
{
    std::underlying_type_t<HAL_StatusTypeDef> status = as_int(HAL_StatusTypeDef::HAL_OK);

    status |= HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);

    status |= !low_level_configuration(param_1);

    return status == as_int(HAL_StatusTypeDef::HAL_OK);
}
int32_t adc_interface_t::read_bridge_voltage()
{
    std::underlying_type_t<HAL_StatusTypeDef> status = as_int(HAL_StatusTypeDef::HAL_OK);

    status |= HAL_ADC_Stop(&hadc);
    status |= as_int(configure_adc_channel(bridge_out_1_channel));
    status |= HAL_ADC_Start(&hadc);
    status |= HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
    int32_t adc_1_value = HAL_ADC_GetValue(&hadc); // Bridge out 1

    status |= HAL_ADC_Stop(&hadc);
    status |= configure_adc_channel(bridge_out_2_channel);
    status |= HAL_ADC_Start(&hadc);
    status |= HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
    int32_t adc_2_value = HAL_ADC_GetValue(&hadc); // Bridge out 2

    int32_t bridge_difference = adc_2_value - adc_1_value;

    return status == as_int(HAL_StatusTypeDef::HAL_OK) ? bridge_difference : -1;
}
bool adc_interface_t::sleep_interface()
{
    std::underlying_type_t<HAL_StatusTypeDef> status = as_int(HAL_StatusTypeDef::HAL_OK);

    status |= HAL_ADC_Stop(&hadc);
    status |= ADC_Disable(&hadc);

    status |= !low_level_sleep();

    return status == as_int(HAL_StatusTypeDef::HAL_OK);
}

HAL_StatusTypeDef adc_interface_t::configure_adc_channel(uint32_t channel)
{

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel                = channel;
    sConfig.Rank                   = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime           = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff             = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber           = ADC_OFFSET_NONE;
    sConfig.Offset                 = 0;
    return HAL_ADC_ConfigChannel(&hadc, &sConfig);
}
