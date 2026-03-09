#include <cstdint>
#include <main.h>
#include <type_traits>

#pragma once

template <typename T> inline constexpr T ceil_div(T a, T b)
{
    static_assert(std::is_integral_v<T>, "T must be integral");
    return (a + b - 1) / b;
}

template <typename Enum> inline constexpr std::underlying_type_t<Enum> as_int(Enum e) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}

extern uint32_t cycles_per_us;
extern uint32_t max_delay_us;

void init_dwt();
void delay_us(uint32_t us);
uint32_t get_dwt_micros();