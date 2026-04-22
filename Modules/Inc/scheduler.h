#include <list>

#include "command.h"
#include "device.h"
#include "timmer_interface.h"

#pragma once

class scheduler_t : public device_t
{
    std::list<command_t> scheduled_commands;
    bool pending_execution;

  public:
    timmer_interface_t scheduler_timer;

    scheduler_t(module_id_t id, TIM_HandleTypeDef *_htim) : device_t(id), scheduler_timer(_htim, elapsed_callback, this)
    {
    }

    bool init() override;

    void handle() override;
    bool execute_command(command_t command, [[maybe_unused]] uint16_t internal_parm = 0) override;

    static void elapsed_callback(void *context);
};
extern scheduler_t *g_scheduler;