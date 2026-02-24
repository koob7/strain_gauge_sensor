#include "device.h"

#pragma once

class serializer_t : public device_t
{
  public:
    serializer_t(module_id_t id) : device_t(id) {}

    void init() { state = state_t::READY; }
    void handle() {}
};

extern serializer_t *g_serializer;