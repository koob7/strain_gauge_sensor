#include "cstring"
#include "usart.h"
#include <optional>
#include <utility>

#include "device.h"

#pragma once

class usart_control_t : public device_t
{
  private:
    static constexpr uint16_t BUFFER_SIZE = 100;
    static constexpr uint64_t TIMEOUT_TRUST_FACTOR = 12;
    static constexpr uint64_t DIVIDER_HELPER_FACTOR = 1000000000000ULL;
    static constexpr uint64_t TIMEOUT_TRUST_DIVIDER = 10;
    static constexpr uint64_t USART_START_BITS = 1;
    static constexpr uint64_t MIN_TO_MS = 1000;
    uint16_t timeout_ms = 0;

    uint8_t tx_buffer[BUFFER_SIZE];
    uint8_t rx_buffer[BUFFER_SIZE];

    UART_HandleTypeDef *huart;
    DMA_HandleTypeDef *hdma_rx;
    DMA_HandleTypeDef *hdma_tx;

    bool data_received;

  public:
    explicit usart_control_t(module_id_t id, UART_HandleTypeDef *uart);

    void init();
    void handle() {}

    void dma_rx_irq(UART_HandleTypeDef *irq_huart);
    void dma_tx_irq(UART_HandleTypeDef *irq_huart);

    bool send_frame(const char *text);
    bool receive_frame();
    std::optional<std::pair<uint8_t *, uint8_t>> read_frame();

    bool check_data_ready() { return data_received; }
};

extern usart_control_t *g_usart_control;