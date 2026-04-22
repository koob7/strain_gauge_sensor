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
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "gpio.h"
#include "opamp.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cstring"
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

#include "command.h"
#include "multiplekser.h"
#include "scheduler.h"
#include "serializer.h"
#include "usart_control.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

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

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
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
    MX_CRC_Init();
    MX_ADC1_Init();
    MX_ADC2_Init();
    MX_TIM1_Init();
    /* USER CODE BEGIN 2 */

    init_dwt();

    static usart_control_t usart_control_instance(device_t::module_id_t::USER_MESSENGER, &huart1);
    g_usart_control = &usart_control_instance;

    static scheduler_t scheduler_instance(device_t::module_id_t::SCHEDULER, &htim1);
    g_scheduler = &scheduler_instance;

    static serializer_t serializer_instance(device_t::module_id_t::SERIALIZER);
    g_serializer = &serializer_instance;

    bool init_result = true;
    for (uint8_t i = 0; i < static_cast<uint8_t>(device_t::module_id_t::MODULE_NUMBER); i++)
    {
        init_result &= g_device_modules[i]->init();
    }

    if (!init_result)
    {
        g_usart_control->send_frame("Critical error during initialization, jump to error handler\n");
        Error_Handler();
    }

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    g_usart_control->receive_frame();

    while (1)
    {
        if (!HAL_GPIO_ReadPin(USER_BTN_GPIO_Port, USER_BTN_Pin))
            HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);

        if (g_usart_control->check_data_ready())
        {
            bool result = false;
            command_t command;
            if (command.decode_frame(g_usart_control->read_frame()) == false)
            {
                goto finish_receiving;
            }

            for (uint8_t i = 0; i < command_t::num_of_internal_commands; i++)
            {
                // Internal command schould not be called by user
                if (as_int(command_t::internal_commands[i]) ==
                    command.parameters[as_int(command_t::default_command_layout_t::COMMAND_ID)])
                {
                    goto finish_receiving;
                }
            }

            for (uint8_t i = 0; i < static_cast<uint8_t>(device_t::module_id_t::MODULE_NUMBER); i++)
            {
                result |= g_device_modules[i]->execute_command(command);
            }

        finish_receiving:

            g_usart_control->send_frame("Command executed with result: %d\n", result);
            g_usart_control->receive_frame();
        }

        for (uint8_t i = 0; i < static_cast<uint8_t>(device_t::module_id_t::MODULE_NUMBER); i++)
        {
            g_device_modules[i]->handle();
        }

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
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = RCC_PLLM_DIV4;
    RCC_OscInitStruct.PLL.PLLN            = 85;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV16;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    g_scheduler->scheduler_timer.elapsed_interrupt(htim);
}

extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) { g_usart_control->dma_tx_irq(huart); }

extern "C" void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (Size == 0)
    {
        g_usart_control->receive_frame();
        return;
    }
    g_usart_control->dma_rx_irq(huart);
}

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
    if (g_usart_control->get_state() != state_t::READY)
    {
        g_usart_control->send_frame("Critical error, jump to error handler\n");
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
