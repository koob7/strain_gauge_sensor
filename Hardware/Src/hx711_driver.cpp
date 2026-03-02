#include "hx711_driver.h"
#include "main.h"

bool hx711_driver::is_ready()
{
    // When the data line goes low, the sensor is ready for retrieval.
    return HAL_GPIO_ReadPin(data_line_port, data_line_pin) == 0;
}

bool hx711_driver::set_gain(gain_ranges_t gain_range)
{

    if (gain >= as_int(gain_ranges_t::GAIN_RANGES_NUMBER))
        return false;

    // we do not have to configure gain so making extra read
    if (gain == gain_sck_pulses[as_int(gain_range)])
    {
        return true;
    }

    gain = gain_sck_pulses[as_int(gain_range)];

    power_up();
    read();
    power_down();
    return true;
}

long hx711_driver::read()
{
    uint32_t start = HAL_GetTick();

    // It checks the start sequence condition.
    while (!is_ready())
    {
        if ((HAL_GetTick() - start) >= TIME_TO_ACTIVE)
        {
            return 0;
        }
    }

    unsigned long value = 0;
    // 3 array each of 8 bit contain 24 bit data.
    uint8_t data[3] = {0};
    // The filler is used for preventing conflict if any in the case of negative number representation.
    uint8_t filler = 0x00;

    for (int j = 0; j < 3; j++)
    {
        for (int i = 0; i < 8; i++)
        {

            // The clock line is set.
            HAL_GPIO_WritePin(clock_line_port, clock_line_pin, GPIO_PIN_SET);
            // The obtaining data is shifted(MSB first).
            data[2 - j] |= HAL_GPIO_ReadPin(data_line_port, data_line_pin) << (7 - i);
            // The clock line is reset.
            HAL_GPIO_WritePin(clock_line_port, clock_line_pin, GPIO_PIN_RESET);
        }
    }

    // Channel and the gain factor are set for the next reading.
    for (int i = 0; i < gain; i++)
    {

        HAL_GPIO_WritePin(clock_line_port, clock_line_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(clock_line_port, clock_line_pin, GPIO_PIN_RESET);
    }

    // It replicates the most significant bit to pad out a 32-bit signed integer.
    if (data[2] & 0x80)
    {
        filler = 0xFF;
    }
    else
    {
        filler = 0x00;
    }
    // It constructs a 32-bit signed integer.
    value = ((unsigned long)(filler) << 24 | (unsigned long)(data[2]) << 16 | (unsigned long)(data[1]) << 8 |
             (unsigned long)(data[0]));

    return (long)(value);
}

bool hx711_driver::power_down()
{

    HAL_GPIO_WritePin(clock_line_port, clock_line_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(clock_line_port, clock_line_pin, GPIO_PIN_SET);

    delay_us(TIME_TO_SLEEP);

    return !is_ready();
}

bool hx711_driver::power_up()
{
    HAL_GPIO_WritePin(clock_line_port, clock_line_pin, GPIO_PIN_RESET);

    uint32_t start = HAL_GetTick();

    while (((HAL_GetTick() - start) <= TIME_TO_ACTIVE) && !is_ready())
    {
    };

    return is_ready();
}