#include "adc.h"
#include "adc_interface.h"
#include "interface.h"
#include "main.h"
#include "stm32g4xx_hal_adc.h"

class adc_temp_t : public adc_interface_t
{
    static constexpr int32_t KOHM_TO_OHM = 1000;
    int32_t helper_resistance            = 1;

  public:
    adc_temp_t() = delete;
    adc_temp_t(interface_id_t id) : adc_interface_t(id, 2, hadc1, ADC_CHANNEL_12, ADC_CHANNEL_VREFINT) {}
    const char *get_char_name() override;
    int32_t read_bridge_voltage() override;

  private:
    bool low_level_wake_up() override;
    bool low_level_configuration(uint8_t param) override;
    bool low_level_sleep() override;
    int32_t convert_voltage_to_resistane(int32_t voltage, int32_t ref_voltage, int32_t resistance);
    int32_t convert_resistance_to_temperature(int32_t resistance);
};