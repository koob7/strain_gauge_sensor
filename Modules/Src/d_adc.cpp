#include "d_adc.h"
#include "adc.h"
#include "main.h"
#include "stm32g4xx_hal_adc.h"

bool adc_t::init()
{
    state = state_t::READY;
    return true;
}

bool adc_t::wake_up_interface() { return true; }
bool adc_t::configure_interface(uint8_t param_1)
{
    // ADC_Enable
    // ADC_Disable
    // HAL_StatusTypeDef ADC_Enable(ADC_HandleTypeDef *hadc);
    // HAL_ADC_ConfigChannel
    // HAL_ADC_DeInit
    // HAL_ADCEx_Calibration_Start(&hadc1);

    // HAL_ADCEx_EnterADCDeepPowerDownMode -> HAL_ADC_Init -> HAL_ADCEx_Calibration_Start
    return true;
}
int32_t adc_t::read_bridge_voltage()
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return adc_value;
}
bool adc_t::sleep_interface() { return true; }