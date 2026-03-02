
#include <cstdint>

#pragma once

#define _ms *1000
#define _s *1000
#define _min *60_s
#define _h *60_min

enum class state_t : uint8_t
{
    INIT    = 0,
    READY   = 1,
    PENDING = 2,
    ERROR   = 3,
};
