#include "temp_module.h"

const char *adc_temp_t::get_char_name() { return "adc temperature"; }
bool adc_temp_t::low_level_wake_up() { return true; }
bool adc_temp_t::low_level_configuration(uint8_t param)
{
    if (param == 0)
        return false;

    helper_resistance = param * KOHM_TO_OHM;
    return true;
}
bool adc_temp_t::low_level_sleep() { return true; }

int32_t adc_temp_t::read_bridge_voltage()
{
    std::underlying_type_t<HAL_StatusTypeDef> status = as_int(HAL_StatusTypeDef::HAL_OK);

    status |= HAL_ADC_Stop(&hadc);
    status |= as_int(configure_adc_channel(bridge_out_1_channel));
    status |= HAL_ADC_Start(&hadc);
    status |= HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
    int32_t adc_1_value = HAL_ADC_GetValue(&hadc); // Bridge out 1

    status |= HAL_ADC_Stop(&hadc);
    status |= as_int(configure_adc_channel(bridge_out_2_channel));
    status |= HAL_ADC_Start(&hadc);
    status |= HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
    int32_t adc_2_value = HAL_ADC_GetValue(&hadc); // Bridge out 2

    if (status != as_int(HAL_StatusTypeDef::HAL_OK))
    {
        return INT32_MIN;
    }

    int32_t resistance = convert_voltage_to_resistane(adc_1_value, adc_2_value, helper_resistance);
    if (resistance < 0)
        return INT32_MIN;

    return convert_resistance_to_temperature(resistance);
}

int32_t adc_temp_t::convert_voltage_to_resistane(int32_t voltage, int32_t ref_voltage, int32_t resistance)
{
    static constexpr int64_t helper = 1000000;

    int64_t numerator = ref_voltage * resistance;
    numerator *= helper;

    int64_t result = (numerator / voltage) / helper - resistance;

    if (result < 0 || result > INT32_MAX)
    {
        return INT32_MIN;
    }

    return static_cast<int32_t>(result);
}

int32_t adc_temp_t::convert_resistance_to_temperature(int32_t resistance)
{
    static constexpr int64_t helper             = 1000000;
    static constexpr int64_t C_TO_MILIC         = 1000;
    static constexpr int64_t MIN_TEMPERATURE    = -20 * C_TO_MILIC;
    static constexpr int64_t TABLE_STEP         = 1 * C_TO_MILIC;
    static constexpr int64_t resistance_table[] = {
        69693, 66329, 63149, 60140, 57294, 54600, 52049, 49633, 47344, 45174, 43117, 41166, 39315, 37559, 35891,
        34307, 32803, 31373, 30015, 28723, 27494, 26325, 25212, 24153, 23144, 22184, 21268, 20396, 19564, 18771,
        18015, 17294, 16605, 15948, 15320, 14720, 14148, 13600, 13077, 12577, 12099, 11641, 11204, 10785, 10384,
        10000, 9632,  9280,  8943,  8620,  8310,  8012,  7728,  7454,  7192,  6940,  6699,  6467,  6244,  6030,
        5825,  5628,  5438,  5256,  5080,  4912,  4750,  4594,  4444,  4300,  4161};
    static constexpr int64_t TABLE_SIZE = sizeof(resistance_table) / sizeof(resistance_table[0]);

    if (resistance <= 0)
        return INT32_MIN;

    if (resistance > resistance_table[0])
        return INT32_MIN;

    for (int i = 0; i < TABLE_SIZE - 1; i++)
    {
        if (resistance > resistance_table[i] && resistance <= resistance_table[i + 1])
        {
            int32_t base_temp    = MIN_TEMPERATURE + i * TABLE_STEP;
            int32_t precise_temp = (((resistance - resistance_table[i]) * TABLE_STEP * helper) /
                                    (resistance_table[i + 1] - resistance_table[i])) /
                                   helper;
            return static_cast<int32_t>(base_temp + precise_temp);
        }
    }
    return INT32_MIN;
}