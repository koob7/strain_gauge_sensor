#include "command.h"
#include "types.h"

#pragma once

class device_t
{

  public:
    enum class module_id_t : uint8_t
    {

        USER_MESSENGER,
        SCHEDULER,
        SERIALIZER,

        // must stay last
        MODULE_NUMBER,
    };

    module_id_t module_id;
    state_t state;

    device_t() = delete;
    device_t(module_id_t id) : module_id(id), state(state_t::INIT) { register_module(); }

    virtual bool init()   = 0;
    virtual void handle() = 0;
    virtual bool execute_command(command_t command, [[maybe_unused]] uint16_t internal_parm = 0) { return false; }
    void error_handler();
    state_t const get_state() { return state; }
    module_id_t const get_module_id() { return module_id; }

  private:
    void register_module();
};

extern device_t *g_device_modules[static_cast<uint8_t>(device_t::module_id_t::MODULE_NUMBER)];