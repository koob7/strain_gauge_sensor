#include "helper.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <type_traits>
#include <utility>

#pragma once

class command_t
{

    static constexpr const char *CMD_PREFIX     = "cmd";
    static constexpr const char *HEX_PREFIX     = "0x";
    static constexpr const char *BINARY_PREFIX  = "0b";
    static constexpr const char *DECIMAL_PREFIX = "";

  public:
    // This enum represent most common and frequenty used layout of command parameters. There is no obstacle to use
    // parameters in different purpose
    enum class default_command_layout_t : uint8_t
    {
        COMMAND_ID,
        INTERFACE,
        BRIDGE_1,
        BRIDGE_2,
        BRIDGE_3,
        BRIDGE_4,
        EXTRA_PARAM,

        // must be last
        PARAMETERS_COUNT,
    };

    uint16_t parameters[as_int(default_command_layout_t::PARAMETERS_COUNT)];

    using parameters_type = typename std::remove_reference_t<decltype(parameters[0])>;

    enum class command_code_t : parameters_type
    {
        // private commands schould be added to internal_commands table

        SCHEDULE_MEASUREMENT          = 0,
        EXECUTE_MEASUREMENTS          = 1,
        SINGLE_MEASUREMENT            = 2,
        REMOVE_MEASUREMENT            = 3,
        REMOVE_SCHEDULED_MEASUREMENTS = 4,
        SERIALIZE_COMMANDS            = 5,
        SAVE_COMMAND_TO_FLASH         = 6, // private
        RESTORE_SERIALIZED_COMMANDS   = 7,
        ERASE_FLASH                   = 8,
        PRINT_SCHEDULED_COMMANDS      = 9,
        PERIODIC_MEASUREMENTS         = 10,
    };

    // dummy static assert, just to learn something and enjoy coding
    static_assert(
        std::is_same_v<std::remove_reference_t<decltype(parameters[0])>, std::underlying_type_t<command_code_t>>,
        "Typ elementu parameters i enum command_code_t nie zgadza się!");

    static_assert(std::is_same_v<std::remove_reference_t<decltype(parameters[0])>,
                                 std::underlying_type_t<decltype(command_code_t::SCHEDULE_MEASUREMENT)>>,
                  "Typ elementu parameters i enum command_code_t nie zgadza się!");

    static constexpr command_code_t internal_commands[]{command_code_t::SAVE_COMMAND_TO_FLASH};
    static constexpr uint8_t num_of_internal_commands = sizeof(internal_commands) / sizeof(internal_commands[0]);

    command_t() { memset(parameters, 0, sizeof(parameters)); }
    command_t(const command_t &command) { memcpy(parameters, command.parameters, sizeof(parameters)); }

    uint16_t read_numberf_from_string(char *&ptr);

    bool decode_frame(std::optional<std::pair<uint8_t *, uint8_t>> frame);

    [[nodiscard]] uint16_t *get_parameters_pointer() { return parameters; }
    [[nodiscard]] static uint8_t get_parameters_size() { return sizeof(parameters); }
    void print_as_dafult_layout();
    void print_as_plain_parameters();
};