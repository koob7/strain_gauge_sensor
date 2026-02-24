#include "interface.h"
#include "usart_control.h"

interface_t *g_interfaces[static_cast<uint8_t>(interface_t::interface_id_t::INTERFACE_NUMBER)] = {};

void interface_t::register_interface()
{
    if (g_interfaces[static_cast<uint8_t>(interface_id)] != nullptr)
    {
        g_usart_control->send_frame("ERR: second interface initiated\n");
        Error_Handler();
    }
    g_interfaces[static_cast<uint8_t>(interface_id)] = this;
}