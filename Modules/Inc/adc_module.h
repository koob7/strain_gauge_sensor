#include "adc.h"
#include "adc_interface.h"
#include "interface.h"
#include "main.h"
#include "stm32g4xx_hal_adc.h"

class adc_t : public adc_interface_t
{
  public:
    adc_t() = delete;
    adc_t(interface_id_t id) : adc_interface_t(id, 2, hadc1, ADC_CHANNEL_12, ADC_CHANNEL_15) {}
    const char *get_char_name() override;

  private:
    bool low_level_wake_up() override;
    bool low_level_configuration(uint8_t param) override;
    bool low_level_sleep() override;
};