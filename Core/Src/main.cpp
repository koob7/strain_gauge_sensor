/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "gpio.h"
#include "opamp.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cstring"
#include <optional>
#include <utility>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define _s *1000
#define _min *60_s
#define _h *60_min
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

class usart_control_t
{
  private:
    static constexpr uint16_t BUFFER_SIZE = 100;
    static constexpr uint64_t TIMEOUT_TRUST_FACTOR = 12;
    static constexpr uint64_t DIVIDER_HELPER_FACTOR = 1000000;
    static constexpr uint64_t TIMEOUT_TRUST_DIVIDER = 10;
    static constexpr uint64_t USART_START_BITS = 1;
    uint16_t timeout_ms = 0;

    uint8_t tx_buffer[BUFFER_SIZE];
    uint8_t rx_buffer[BUFFER_SIZE];

    UART_HandleTypeDef *huart;
    DMA_HandleTypeDef *hdma_rx;
    DMA_HandleTypeDef *hdma_tx;

    bool data_received;
    bool initiated = false;

  public:
    explicit usart_control_t() {}

    explicit usart_control_t(UART_HandleTypeDef *uart) : huart(uart)
    {
        if (uart == nullptr)
            Error_Handler();

        if (uart->hdmarx == nullptr || uart->hdmatx == nullptr)
            Error_Handler();

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
            Error_Handler();
        }

        uint64_t parity_bits = uart->Init.Parity == UART_PARITY_NONE ? 0 : 1;

        timeout_ms = static_cast<uint16_t>(DIVIDER_HELPER_FACTOR / uart->Init.BaudRate * BUFFER_SIZE *
                                           (USART_START_BITS + word_length + parity_bits + stop_bits) *
                                           TIMEOUT_TRUST_FACTOR / DIVIDER_HELPER_FACTOR / TIMEOUT_TRUST_DIVIDER);

        data_received = false;
        initiated = true;
    }

    bool check_initiated() { return initiated; }

    void dma_rx_irq(UART_HandleTypeDef *irq_huart)
    {
        if (irq_huart != huart)
        {
            return;
        }

        data_received = true;
    }

    void dma_tx_irq(UART_HandleTypeDef *irq_huart)
    {
        if (irq_huart != huart)
        {
            return;
        }
    }

    bool send_frame(const char *text)
    {

        uint32_t start = HAL_GetTick();

        while (huart->gState != HAL_UART_STATE_READY)
        {
            if ((HAL_GetTick() - start) >= timeout_ms)
            {
                return false;
            }
        }

        size_t len = strlen(text);

        if (len == 0 || len > BUFFER_SIZE)
            return false;

        memcpy(tx_buffer, text, len);

        if (HAL_UART_Transmit_DMA(huart, tx_buffer, len) != HAL_OK)
            return false;

        return true;
    }

    bool receive_frame()
    {

        uint32_t start = HAL_GetTick();
        data_received = 0;
        while (huart->RxState != HAL_UART_STATE_READY)
        {
            if ((HAL_GetTick() - start) >= timeout_ms)
            {
                return false;
            }
        }

        if (HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer, BUFFER_SIZE) != HAL_OK)
        {
            return false;
        }

        return true;
    }

    std::optional<std::pair<uint8_t *, uint8_t>> read_frame()
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
        received = BUFFER_SIZE - remaining;

        if (received == 0 || received > 100)
            return std::nullopt;

        return std::make_optional(std::make_pair(rx_buffer, received));
    }

    bool check_data_ready() { return data_received; }
};

usart_control_t user_messenger;
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU
     * Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the
     * Systick.
     */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_OPAMP1_Init();
    MX_OPAMP2_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */

    user_messenger = usart_control_t(&huart1);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        if (!HAL_GPIO_ReadPin(USER_BTN_GPIO_Port, USER_BTN_Pin))
            HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
    RCC_OscInitStruct.PLL.PLLN = 85;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

void HAL_UARTEx_TxEventCallback(UART_HandleTypeDef *huart) { user_messenger.dma_tx_irq(huart); }

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart) { user_messenger.dma_rx_irq(huart); }

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state
     */
    if (user_messenger.check_initiated())
    {
        user_messenger.send_frame("Critical error, jump to error handler\n");
    }
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
