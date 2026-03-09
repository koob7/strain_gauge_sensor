#include "helper.h"

uint32_t cycles_per_us = 0;
uint32_t max_delay_us  = 0;

void init_dwt()
{
    CoreDebug->DEMCR = CoreDebug->DEMCR | CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT      = 0;
    DWT->CTRL        = DWT->CTRL | DWT_CTRL_CYCCNTENA_Msk;

    cycles_per_us = SystemCoreClock / 1000000;
    max_delay_us  = UINT32_MAX / cycles_per_us;
}

void delay_us(uint32_t us)
{
    if (us > max_delay_us)
        us = max_delay_us;

    uint32_t cycles = us * cycles_per_us;

    DWT->CYCCNT = 0;

    while ((uint32_t)(DWT->CYCCNT) < cycles)
        ;
}

uint32_t get_dwt_micros() { return DWT->CYCCNT / cycles_per_us; }
