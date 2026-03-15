#include "command.h"
#include "helper.h"
#include "usart_control.h"

uint16_t command_t::read_numberf_from_string(char *&ptr)
{
    if (memcmp(ptr, HEX_PREFIX, strlen(HEX_PREFIX)) == 0)
    {
        ptr += strlen(HEX_PREFIX);
        return static_cast<uint16_t>(strtoul(ptr, &ptr, 16));
    }
    else if (memcmp(ptr, BINARY_PREFIX, strlen(BINARY_PREFIX)) == 0)
    {
        ptr += strlen(BINARY_PREFIX);
        return static_cast<uint16_t>(strtoul(ptr, &ptr, 2));
    }
    else if (memcmp(ptr, DECIMAL_PREFIX, strlen(DECIMAL_PREFIX)) == 0)
    {
        ptr += strlen(DECIMAL_PREFIX);
        return static_cast<uint16_t>(strtoul(ptr, &ptr, 10));
    }

    return 0;
}

bool command_t::decode_frame(std::optional<std::pair<uint8_t *, uint8_t>> frame)
{
    if (!frame.has_value())
    {
        return false;
    }

    const char *base_ptr = reinterpret_cast<const char *>(frame.value().first);
    char *frame_content  = reinterpret_cast<char *>(frame.value().first);
    uint8_t frame_size   = frame.value().second;

    if (frame_content == nullptr)
    {
        return false;
    }

    if (frame_size < strlen(CMD_PREFIX))
    {
        return false;
    }

    if (memcmp(frame_content, CMD_PREFIX, strlen(CMD_PREFIX)) != 0)
    {
        return false;
    }
    frame_content += strlen(CMD_PREFIX);
    frame_content += strlen(" ");

    static_assert(std::is_same_v<std::remove_reference_t<decltype(parameters[0])>, uint16_t>,
                  "niezgodne typy parametrów komend");
    for (uint8_t i = 0; i < sizeof(parameters) && frame_content != nullptr && frame_content - base_ptr <= frame_size;
         i++)
    {
        parameters[i] = read_numberf_from_string(frame_content);

        if (frame_content == nullptr)
        {
            return false;
        }

        if (*frame_content == '\0')
        {
            return true;
        }

        if (*frame_content != ' ')
        {
            return false;
        }

        frame_content += strlen(" ");
    }
    return true;
}

void command_t::print_as_dafult_layout()
{
    using layout_t = command_t::default_command_layout_t;

    g_usart_control->send_frame("Commmand: ");
    g_usart_control->send_frame("Command ID: %d ", parameters[as_int(layout_t::COMMAND_ID)]);
    g_usart_control->send_frame("Interface: %d ", parameters[as_int(layout_t::INTERFACE)]);
    g_usart_control->send_frame("Bridge 1: %d ", parameters[as_int(layout_t::BRIDGE_1)]);
    g_usart_control->send_frame("Bridge 2: %d ", parameters[as_int(layout_t::BRIDGE_2)]);
    g_usart_control->send_frame("Bridge 3: %d ", parameters[as_int(layout_t::BRIDGE_3)]);
    g_usart_control->send_frame("Bridge 4: %d ", parameters[as_int(layout_t::BRIDGE_4)]);
    g_usart_control->send_frame("Extra Param: %d\n", parameters[as_int(layout_t::EXTRA_PARAM)]);
}

void command_t::print_as_plain_parameters()
{
    g_usart_control->send_frame("Command parameters:");
    for (uint8_t i = 0; i < as_int(command_t::default_command_layout_t::PARAMETERS_COUNT); i++)
    {
        g_usart_control->send_frame(" %d", parameters[i]);
    }
    g_usart_control->send_frame("\n");
}