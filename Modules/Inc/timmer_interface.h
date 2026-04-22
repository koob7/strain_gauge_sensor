#include "main.h"
#include "tim.h"

#pragma once

class timmer_interface_t
{
    TIM_HandleTypeDef *htim;

    using callback_t = void (*)(void *context);
    callback_t elapsed_callback;
    void *callback_context;

  public:
    timmer_interface_t(TIM_HandleTypeDef *htim, callback_t callback, void *context)
        : htim(htim), elapsed_callback(callback), callback_context(context)
    {
    }
    timmer_interface_t() = delete;

    void start_timer();
    void stop_timer();
    void set_timer_period(uint32_t period_us);

    void elapsed_interrupt(TIM_HandleTypeDef *_htim);
};