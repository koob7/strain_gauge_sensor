#include <cstdint>
#include <type_traits>

#pragma once

template <typename T> constexpr T ceil_div(T a, T b)
{
    static_assert(std::is_integral_v<T>, "T must be integral");
    return (a + b - 1) / b;
}