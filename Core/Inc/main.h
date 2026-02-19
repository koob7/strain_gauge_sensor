/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MULTI7_E_Pin GPIO_PIN_13
#define MULTI7_E_GPIO_Port GPIOC
#define MULTI7_S1_Pin GPIO_PIN_14
#define MULTI7_S1_GPIO_Port GPIOC
#define MULTI7_S2_Pin GPIO_PIN_15
#define MULTI7_S2_GPIO_Port GPIOC
#define MULTI7_S3_Pin GPIO_PIN_0
#define MULTI7_S3_GPIO_Port GPIOF
#define MULTI8_E_Pin GPIO_PIN_1
#define MULTI8_E_GPIO_Port GPIOF
#define MULTI8_S1_Pin GPIO_PIN_10
#define MULTI8_S1_GPIO_Port GPIOG
#define MULTI8_S2_Pin GPIO_PIN_0
#define MULTI8_S2_GPIO_Port GPIOC
#define MULTI8_S3_Pin GPIO_PIN_1
#define MULTI8_S3_GPIO_Port GPIOC
#define USER_BTN_Pin GPIO_PIN_3
#define USER_BTN_GPIO_Port GPIOC
#define USER_LED_Pin GPIO_PIN_0
#define USER_LED_GPIO_Port GPIOA
#define ADC2_INP_Pin GPIO_PIN_4
#define ADC2_INP_GPIO_Port GPIOC
#define ADC2_INM_Pin GPIO_PIN_5
#define ADC2_INM_GPIO_Port GPIOC
#define ADC1_INM_Pin GPIO_PIN_0
#define ADC1_INM_GPIO_Port GPIOB
#define ADC1_INP_Pin GPIO_PIN_1
#define ADC1_INP_GPIO_Port GPIOB
#define MULTI1_E_Pin GPIO_PIN_11
#define MULTI1_E_GPIO_Port GPIOB
#define MULTI1_S1_Pin GPIO_PIN_12
#define MULTI1_S1_GPIO_Port GPIOB
#define MULTI1_S2_Pin GPIO_PIN_13
#define MULTI1_S2_GPIO_Port GPIOB
#define MULTI1_S3_Pin GPIO_PIN_14
#define MULTI1_S3_GPIO_Port GPIOB
#define MULTI2_E_Pin GPIO_PIN_15
#define MULTI2_E_GPIO_Port GPIOB
#define MULTI2_S1_Pin GPIO_PIN_6
#define MULTI2_S1_GPIO_Port GPIOC
#define MULTI2_S2_Pin GPIO_PIN_7
#define MULTI2_S2_GPIO_Port GPIOC
#define MULTI2_S3_Pin GPIO_PIN_8
#define MULTI2_S3_GPIO_Port GPIOC
#define MULTI3_E_Pin GPIO_PIN_9
#define MULTI3_E_GPIO_Port GPIOC
#define MULTI3_S1_Pin GPIO_PIN_8
#define MULTI3_S1_GPIO_Port GPIOA
#define MULTI3_S2_Pin GPIO_PIN_11
#define MULTI3_S2_GPIO_Port GPIOA
#define MULTI3_S3_Pin GPIO_PIN_12
#define MULTI3_S3_GPIO_Port GPIOA
#define MULTI4_E_Pin GPIO_PIN_15
#define MULTI4_E_GPIO_Port GPIOA
#define MULTI4_S1_Pin GPIO_PIN_10
#define MULTI4_S1_GPIO_Port GPIOC
#define MULTI4_S2_Pin GPIO_PIN_11
#define MULTI4_S2_GPIO_Port GPIOC
#define MULTI4_S3_Pin GPIO_PIN_12
#define MULTI4_S3_GPIO_Port GPIOC
#define MULTI5_E_Pin GPIO_PIN_2
#define MULTI5_E_GPIO_Port GPIOD
#define MULTI5_S1_Pin GPIO_PIN_3
#define MULTI5_S1_GPIO_Port GPIOB
#define MULTI5_S2_Pin GPIO_PIN_4
#define MULTI5_S2_GPIO_Port GPIOB
#define MULTI5_S3_Pin GPIO_PIN_5
#define MULTI5_S3_GPIO_Port GPIOB
#define MULTI6_E_Pin GPIO_PIN_6
#define MULTI6_E_GPIO_Port GPIOB
#define MULTI6_S1_Pin GPIO_PIN_7
#define MULTI6_S1_GPIO_Port GPIOB
#define MULTI6_S2_Pin GPIO_PIN_8
#define MULTI6_S2_GPIO_Port GPIOB
#define MULTI6_S3_Pin GPIO_PIN_9
#define MULTI6_S3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
