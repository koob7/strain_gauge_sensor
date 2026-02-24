#include "device.h"

#pragma once

class scheduler_t : public device_t
{
    // std::vector<command_t> command_list;

  public:
    scheduler_t(module_id_t id) : device_t(id) {}

    void init() override;

    void handle() {}
};
extern scheduler_t *g_scheduler;