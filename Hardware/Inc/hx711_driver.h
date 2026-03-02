#include <cstdint>

#include "helper.h"
#include "main.h"

#pragma once

class hx711_driver
{

    static constexpr uint32_t TIME_TO_SLEEP  = 65;  // time in us until hx711 enter power down mode when SCK is high
    static constexpr uint32_t TIME_TO_ACTIVE = 500; // time in ms until hx711 enter normal mode when SCK is low

    int gain;
    float scale; // TODO replace to integer (full number)

    GPIO_TypeDef *data_line_port;
    uint16_t data_line_pin;

    GPIO_TypeDef *clock_line_port;
    uint16_t clock_line_pin;

  public:
    enum class gain_ranges_t
    {
        GAIN_128 = 0,
        GAIN_64  = 1,
        // GAIN_32  = 3, Inactive, it's used for channel B

        // must stay last
        GAIN_RANGES_NUMBER,
    };

    // clang-format off
    static constexpr uint8_t gain_sck_pulses[as_int(gain_ranges_t::GAIN_RANGES_NUMBER)] = {
        [as_int(gain_ranges_t::GAIN_128)] = 1, 
        [as_int(gain_ranges_t::GAIN_64)]  = 3,
        //[as_int(gain_ranges_t::GAIN_32)] = 2,
    };
    // clang-format on

    explicit hx711_driver(gain_ranges_t gain_in, GPIO_TypeDef *data_line_port, uint16_t data_line_pin,
                          GPIO_TypeDef *clock_line_port, uint16_t clock_line_pin)
        : data_line_port(data_line_port), data_line_pin(data_line_pin), clock_line_port(clock_line_port),
          clock_line_pin(clock_line_pin)
    {
        if (data_line_port == nullptr || clock_line_port == nullptr)
        {
            Error_Handler();
        }

        set_gain(gain_in);
    }

    bool set_gain(gain_ranges_t gain_range); // Sets the gain factor.
    long read();                             // Waits for the chip to be ready and returns a reading.
    bool power_down();                       // Puts the sensor into the power down mode.
    bool power_up();                         // Wakes up the sensor and returns it to normal operation mode.

  private:
    bool is_ready(); // Checks whether the sensor is ready for retrieval.
};
