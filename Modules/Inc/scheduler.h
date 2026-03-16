#include <list>

#include "command.h"
#include "device.h"

#pragma once

class scheduler_t : public device_t
{
    std::list<command_t> scheduled_commands;

  public:
    scheduler_t(module_id_t id) : device_t(id) {}

    bool init() override;

    void handle() override;
    bool execute_command(command_t command, [[maybe_unused]] uint16_t internal_parm = 0) override;
};
extern scheduler_t *g_scheduler;