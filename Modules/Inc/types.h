
#include <cstdint>

#pragma once

#define MS_TO_US 1000
#define SEC_TO_MS 1000
#define MIN_TO_SEC 60
#define HOUR_TO_MIN 60

enum class state_t : uint8_t
{
    INIT    = 0,
    READY   = 1,
    PENDING = 2,
    ERROR   = 3,
};
