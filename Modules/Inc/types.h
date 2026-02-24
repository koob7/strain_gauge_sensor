
#include <cstdint>

#pragma once

enum class state_t : uint8_t
{
    INIT = 0,
    READY = 1,
    PENDING = 2,
    ERROR = 3,
};
