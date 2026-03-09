#include "interface.h"

#pragma once

class adc_interface_t : public interface_t
{
  public:
    ADC_HandleTypeDef hadc;

    uint32_t bridge_out_1_channel;
    uint32_t bridge_out_2_channel;

    adc_interface_t(interface_id_t id, uint8_t multiplekser_ouput, ADC_HandleTypeDef hadc, uint32_t channel_1,
                    uint32_t channel_2)
        : interface_t(id, multiplekser_ouput), hadc(hadc), bridge_out_1_channel(channel_1),
          bridge_out_2_channel(channel_2)
    {
    }
    adc_interface_t() = delete;
    bool init() override;

    bool wake_up_interface() override;
    bool configure_interface(uint8_t param_1) override;
    int32_t read_bridge_voltage() override;
    bool sleep_interface() override;

    virtual bool low_level_wake_up()                    = 0;
    virtual bool low_level_configuration(uint8_t param) = 0;
    virtual bool low_level_sleep()                      = 0;
    HAL_StatusTypeDef configure_adc_channel(uint32_t channel);
};