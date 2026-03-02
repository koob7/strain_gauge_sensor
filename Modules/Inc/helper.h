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

static uint32_t cycles_per_us;
static uint32_t max_delay_us;

inline void init_dwt()
{
    CoreDebug->DEMCR = CoreDebug->DEMCR | CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT      = 0;
    DWT->CTRL        = DWT->CTRL | DWT_CTRL_CYCCNTENA_Msk;

    cycles_per_us = SystemCoreClock / 1000000;
    max_delay_us  = 0xFFFFFFFF / cycles_per_us;
}

inline void delay_us(uint32_t us)
{
    if (us > max_delay_us)
        us = max_delay_us;

    uint32_t start  = DWT->CYCCNT;
    uint32_t cycles = us * cycles_per_us;

    DWT->CYCCNT = 0;

    while ((uint32_t)(DWT->CYCCNT - start) < cycles)
        ;
}

inline uint32_t get_dwt_micros() { return DWT->CYCCNT / cycles_per_us; }