#include <cstdint>

#include "helper.h"
#include "main.h"

#pragma once

class multiplekser_t
{

  public:
    enum class multiplekser_name_t : uint8_t
    {
        BRIDGE_1            = 0,
        BRIDGE_2            = 1,
        BRIDGE_3            = 2,
        BRIDGE_4            = 3,
        MEASSURING_SOURCE_P = 4,
        MEASSURING_SOURCE_N = 5,
        OPAMP_GAIN          = 6,

        // must stay last
        MULTIPLEKSER_NUMBER,
    };

  private:
    GPIO_TypeDef *s1_port;
    uint16_t s1_pin;

    GPIO_TypeDef *s2_port;
    uint16_t s2_pin;

    GPIO_TypeDef *s3_port;
    uint16_t s3_pin;

    GPIO_TypeDef *enable_port;
    uint16_t enable_pin;

    const multiplekser_name_t multiplekser_type;
    const uint8_t multiplekser_outputs;

    // clang-format off
    static constexpr uint8_t multiplekser_max_out_channel[as_int(multiplekser_name_t::MULTIPLEKSER_NUMBER)] = {
        [as_int(multiplekser_name_t::BRIDGE_1)]            = 16,
        [as_int(multiplekser_name_t::BRIDGE_2)]            = 8,
        [as_int(multiplekser_name_t::BRIDGE_3)]            = 8,
        [as_int(multiplekser_name_t::BRIDGE_4)]            = 8,
        [as_int(multiplekser_name_t::MEASSURING_SOURCE_P)] = 3,
        [as_int(multiplekser_name_t::MEASSURING_SOURCE_N)] = 3,
        [as_int(multiplekser_name_t::OPAMP_GAIN)]          = 8,
    };
    // clang-format on

    static constexpr uint8_t multiplekser_output_number = 8;

    static multiplekser_name_t validate(multiplekser_name_t name)
    {
        if (name >= multiplekser_name_t::MULTIPLEKSER_NUMBER)
        {
            Error_Handler();
        }
        return name;
    }

  public:
    explicit multiplekser_t(multiplekser_name_t multiplekser_name)
        : multiplekser_type(validate(multiplekser_name)),
          multiplekser_outputs(multiplekser_t::multiplekser_max_out_channel[static_cast<uint8_t>(multiplekser_name)])
    {
    }

    bool deactivate_multiplekser();
    bool config_output_channel(uint8_t output_channel);

  private:
    bool config_pin_pointers(uint8_t output_channel);

    bool enable_multiplekser_output(uint8_t output_channel);

    bool safe_HALL_pin_set(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
};

extern multiplekser_t g_multipleksers[as_int(multiplekser_t::multiplekser_name_t::MULTIPLEKSER_NUMBER)];