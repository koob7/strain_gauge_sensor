#include "device.h"

#pragma once

class serializer_t : public device_t
{
  public:
    serializer_t(module_id_t id) : device_t(id) {}

    bool init() override;
    void handle() {}
};

extern serializer_t *g_serializer;