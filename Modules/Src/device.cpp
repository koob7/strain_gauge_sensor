#include "device.h"
#include "main.h"
#include "usart_control.h"

device_t *g_device_modules[static_cast<uint8_t>(device_t::module_id_t::MODULE_NUMBER)] = {};

void device_t::error_handler() { Error_Handler(); }

void device_t::register_module()
{
    if (g_device_modules[static_cast<uint8_t>(module_id)] != nullptr)
    {
        g_usart_control->send_frame("ERR: second module initiated\n");
        Error_Handler();
    }
    g_device_modules[static_cast<uint8_t>(module_id)] = this;
}