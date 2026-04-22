#include "timmer_interface.h"

void timmer_interface_t::start_timer() { HAL_TIM_Base_Start_IT(htim); }
void timmer_interface_t::stop_timer() { HAL_TIM_Base_Stop_IT(htim); }
void timmer_interface_t::set_timer_period(uint32_t period_us)
{
    uint64_t timer_clock = 0;
    switch ((uint32_t)htim->Instance) // Instance store base addres of the timer
    {
    case TIM1_BASE:                               // TIM1_BASE is a base address of TIM1 timer, defined in stm32g476xx.h
        timer_clock = HAL_RCC_GetPCLK2Freq() * 2; // TIM1 is connected to APB2 bus clock by built in multiplyer by 2
        break;
    default:
        Error_Handler();
    }

    uint32_t reload = static_cast<uint32_t>((timer_clock * period_us / 1'000'000) / (htim->Init.Prescaler + 1) - 1);

    __HAL_TIM_SET_AUTORELOAD(htim, reload);
}

void timmer_interface_t::elapsed_interrupt(TIM_HandleTypeDef *_htim)
{
    if (htim->Instance != _htim->Instance)
    {
        return;
    }

    if (elapsed_callback == nullptr)
    {
        return;
    }

    elapsed_callback(callback_context);
}