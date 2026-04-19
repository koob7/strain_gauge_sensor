#include "stm32g4xx_ll_usart.h"
#include <cstdarg>
#include <cstdio>

#include "helper.h"
#include "types.h"
#include "usart_control.h"

usart_control_t *g_usart_control = nullptr;

usart_control_t::usart_control_t(module_id_t id, UART_HandleTypeDef *uart)
    : device_t(id), huart(uart), data_received(false), is_receiving(false)
{
    state = state_t::INIT;
    if (uart == nullptr)
    {
        Error_Handler();
        state = state_t::ERROR;
    }

    if (uart->hdmarx == nullptr || uart->hdmatx == nullptr)
    {
        Error_Handler();
        state = state_t::ERROR;
    }

    hdma_rx = uart->hdmarx;
    hdma_tx = uart->hdmatx;

    uint64_t word_length;

    switch (uart->Init.WordLength)
    {
    case UART_WORDLENGTH_9B:
        word_length = 9;
        break;
    case UART_WORDLENGTH_8B:
        word_length = 8;
        break;
    case UART_WORDLENGTH_7B:
        word_length = 7;
        break;
    default:
        state = state_t::ERROR;
        Error_Handler();
    }

    uint64_t stop_bits;

    switch (uart->Init.StopBits)
    {
    case UART_STOPBITS_0_5:
    case UART_STOPBITS_1:
        stop_bits = 1.0;
        break;

    case UART_STOPBITS_1_5:
    case UART_STOPBITS_2:
        stop_bits = 2;
        break;

    default:
        state = state_t::ERROR;
        Error_Handler();
    }

    uint64_t parity_bits = uart->Init.Parity == UART_PARITY_NONE ? 0 : 1;

    timeout_ms =
        static_cast<uint16_t>(DIVIDER_HELPER_FACTOR / uart->Init.BaudRate * BUFFER_SIZE *
                              (USART_START_BITS + word_length + parity_bits + stop_bits) * TIMEOUT_TRUST_FACTOR *
                              MIN_TO_MS / DIVIDER_HELPER_FACTOR / TIMEOUT_TRUST_DIVIDER);

    data_received = false;
}

bool usart_control_t::init()
{
    if (state != state_t::INIT)
    {
        state = state_t::ERROR;
        return false;
    }
    send_frame("Welcome from STM32G4\n");
    send_frame("In Straing Gauge sensor\n\n\n");
    send_frame("User messenger initiated\n");
    state = state_t::READY;
    return true;
}

bool usart_control_t::execute_command(command_t command, [[maybe_unused]] uint16_t internal_parm)
{
    switch (command.parameters[as_int(command_t::default_command_layout_t::COMMAND_ID)])
    {
    default:
        return false;
    }
    return false;
}

void usart_control_t::dma_rx_irq(UART_HandleTypeDef *irq_huart)
{
    if (irq_huart != huart)
    {
        return;
    }

    if (irq_huart->RxEventType != HAL_UART_RXEVENT_IDLE)
    {
        // DMA callback on half full buffer
        return;
    }

    LL_USART_DisableDirectionRx(huart->Instance);

    is_receiving  = false;
    data_received = true;
}

void usart_control_t::dma_tx_irq(UART_HandleTypeDef *irq_huart)
{
    if (irq_huart != huart)
    {
        return;
    }
}

bool usart_control_t::send_frame(const char *format, ...)
{
    uint32_t start = HAL_GetTick();

    while (huart->gState != HAL_UART_STATE_READY)
    {
        if ((HAL_GetTick() - start) >= timeout_ms)
        {
            return false;
        }
    }

    va_list args;
    va_start(args, format);

    uint16_t len = vsnprintf((char *)tx_buffer, BUFFER_SIZE, format, args);
    va_end(args);

    if (len < 0 || len >= BUFFER_SIZE)
        return false;

    if (HAL_UART_Transmit_DMA(huart, tx_buffer, len) != HAL_OK)
        return false;

    return true;
}

bool usart_control_t::receive_frame()
{
    memset(rx_buffer, 0, sizeof(rx_buffer));

    uint32_t start = HAL_GetTick();
    data_received  = 0;
    while (huart->RxState != HAL_UART_STATE_READY)
    {
        if ((HAL_GetTick() - start) >= timeout_ms)
        {
            return false;
        }
    }

    LL_USART_EnableDirectionRx(huart->Instance);
    if (HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer, BUFFER_SIZE) != HAL_OK)
    {
        return false;
    }

    is_receiving = true;
    return true;
}

std::optional<std::pair<uint8_t *, uint8_t>> usart_control_t::read_frame()
{
    if (huart->RxState != HAL_UART_STATE_READY)
    {
        return std::nullopt;
    }

    if (!data_received)
    {
        return std::nullopt;
    }

    uint32_t remaining;
    uint32_t received;

    remaining = __HAL_DMA_GET_COUNTER(huart->hdmarx);
    received  = BUFFER_SIZE - remaining;

    if (received == 0 || received > 100)
    {
        receive_frame();
        return std::nullopt;
    }
    return std::make_optional(std::make_pair(rx_buffer, received));
}