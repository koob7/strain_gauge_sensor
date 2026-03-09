#include "d_adc.h"
#include "adc.h"
#include "helper.h"
#include "main.h"
#include "stm32g4xx_hal_adc.h"

bool adc_t::init()
{
    state = state_t::READY;

    return true;
}

bool adc_t::wake_up_interface()
{
    std::underlying_type_t<HAL_StatusTypeDef> status = as_int(HAL_StatusTypeDef::HAL_OK);

    status |= as_int(ADC_Enable(&hadc1));
    return status == as_int(HAL_StatusTypeDef::HAL_OK);
}
bool adc_t::configure_interface(uint8_t param_1)
{
    std::underlying_type_t<HAL_StatusTypeDef> status = as_int(HAL_StatusTypeDef::HAL_OK);

    status |= HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);

    return status == as_int(HAL_StatusTypeDef::HAL_OK);
}
int32_t adc_t::read_bridge_voltage()
{
    std::underlying_type_t<HAL_StatusTypeDef> status = as_int(HAL_StatusTypeDef::HAL_OK);

    status |= HAL_ADC_Stop(&hadc1);
    status |= as_int(configure_adc_channel(&hadc1, bridge_out_1_channel));
    status |= HAL_ADC_Start(&hadc1);
    status |= HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    int32_t adc_1_value = HAL_ADC_GetValue(&hadc1); // Bridge out 1

    status |= HAL_ADC_Stop(&hadc1);
    status |= configure_adc_channel(&hadc1, bridge_out_2_channel);
    status |= HAL_ADC_Start(&hadc1);
    status |= HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    int32_t adc_2_value = HAL_ADC_GetValue(&hadc1); // Bridge out 2

    int32_t bridge_difference = adc_2_value - adc_1_value;

    return status == as_int(HAL_StatusTypeDef::HAL_OK) ? bridge_difference : -1;
}
bool adc_t::sleep_interface()
{
    std::underlying_type_t<HAL_StatusTypeDef> status = as_int(HAL_StatusTypeDef::HAL_OK);

    status |= HAL_ADC_Stop(&hadc1);
    status |= ADC_Disable(&hadc1);

    return status == as_int(HAL_StatusTypeDef::HAL_OK);
}

HAL_StatusTypeDef adc_t::configure_adc_channel(ADC_HandleTypeDef *hadc, uint32_t Channel)
{

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel                = Channel;
    sConfig.Rank                   = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime           = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff             = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber           = ADC_OFFSET_NONE;
    sConfig.Offset                 = 0;
    return HAL_ADC_ConfigChannel(hadc, &sConfig);
}