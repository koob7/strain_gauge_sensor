#include "interface.h"

class adc_t : public interface_t
{
    static constexpr uint32_t bridge_out_1_channel = ADC_CHANNEL_12;
    static constexpr uint32_t bridge_out_2_channel = ADC_CHANNEL_15;

  public:
    adc_t(interface_id_t id) : interface_t(id, 2) {}
    bool init();
    const char *get_char_name() { return "adc"; }

  private:
    bool wake_up_interface() override;
    bool configure_interface(uint8_t param_1) override;
    int32_t read_bridge_voltage() override;
    bool sleep_interface() override;
    HAL_StatusTypeDef configure_adc_channel(ADC_HandleTypeDef *hadc, uint32_t Channel);
};