#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <type_traits>
#include <utility>

#pragma once

class command_t
{

    const char *CMD_PREFIX     = "cmd";
    const char *HEX_PREFIX     = "0x";
    const char *BINARY_PREFIX  = "0b";
    const char *DECIMAL_PREFIX = "";

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

    uint8_t parameters[static_cast<uint8_t>(default_command_layout_t::PARAMETERS_COUNT)];

    using parameters_type = typename std::remove_reference_t<decltype(parameters[0])>;

    enum class command_code_t : parameters_type
    {
        SCHEDULE_MEASUREMENT,
        EXECUTE_MEASUREMENTS,
        SINGLE_MEASUREMENT,
        REMOVE_MEASUREMENT,
        REMOVE_ALL_MEASUREMENTS,
        SERIALIZE_COMMANDS,
        RESTORE_SERIALIZED_COMMANDS,
        REMOVE_SERIALIZED_COMMANDS,
    };

    // dummy static assert, just to learn something and enjoy coding
    static_assert(
        std::is_same_v<std::remove_reference_t<decltype(parameters[0])>, std::underlying_type_t<command_code_t>>,
        "Typ elementu parameters i enum command_code_t nie zgadza się!");

    static_assert(std::is_same_v<std::remove_reference_t<decltype(parameters[0])>,
                                 std::underlying_type_t<decltype(command_code_t::SCHEDULE_MEASUREMENT)>>,
                  "Typ elementu parameters i enum command_code_t nie zgadza się!");

    command_t() { memset(parameters, 0, sizeof(parameters)); }
    command_t(const command_t &command) { memcpy(parameters, command.parameters, sizeof(parameters)); }

    uint8_t read_numberf_from_string(char *&ptr);

    bool decode_frame(std::optional<std::pair<uint8_t *, uint8_t>> frame);
};