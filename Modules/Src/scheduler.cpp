#include "scheduler.h"
#include "adc.h"
#include "adc_opamp.h"
#include "hx711.h"
#include "usart_control.h"

scheduler_t *g_scheduler = nullptr;

void scheduler_t::init()
{

    static hx711_t hx711(interface_t::interface_id_t::HX711);
    static adc_opamp_t adc_opamp(interface_t::interface_id_t::ADC_OPAMP);
    static adc_t adc(interface_t::interface_id_t::ADC);

    bool result = true;

    for (uint8_t i = 0; i < static_cast<uint8_t>(interface_t::interface_id_t::INTERFACE_NUMBER); i++)
    {
        result &= g_interfaces[i]->init();
    }

    if (!result)
    {
        state = state_t::ERROR;
        Error_Handler();
    }

    state = state_t::READY;

    g_usart_control->send_frame("command module initiated");
}