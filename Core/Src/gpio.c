/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    gpio.c
 * @brief   This file provides code for the configuration
 *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, MULTI7_S1_Pin|MULTI7_S2_Pin|MULTI7_S3_Pin|MULTI8_S2_Pin
                          |MULTI8_S3_Pin|MULTI2_S1_Pin|MULTI2_S2_Pin|MULTI2_S3_Pin
                          |MULTI3_E_Pin|MULTI4_S1_Pin|MULTI4_S2_Pin|MULTI4_S3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, MULTI8_E_Pin|MULTI8_S1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, USER_LED_Pin|HX711_SCK_Pin|MULTI3_S1_Pin|MULTI3_S2_Pin
                          |MULTI3_S3_Pin|MULTI4_E_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, MULTI7_E_Pin|MULTI1_E_Pin|MULTI1_S1_Pin|MULTI1_S2_Pin
                          |MULTI1_S3_Pin|MULTI2_E_Pin|MULTI5_S1_Pin|MULTI5_S2_Pin
                          |MULTI5_S3_Pin|MULTI6_E_Pin|MULTI6_S1_Pin|MULTI6_S2_Pin
                          |MULTI6_S3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MULTI5_E_GPIO_Port, MULTI5_E_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : MULTI7_S1_Pin MULTI7_S2_Pin MULTI7_S3_Pin MULTI8_S2_Pin
                           MULTI8_S3_Pin MULTI2_S1_Pin MULTI2_S2_Pin MULTI2_S3_Pin
                           MULTI3_E_Pin MULTI4_S1_Pin MULTI4_S2_Pin MULTI4_S3_Pin */
  GPIO_InitStruct.Pin = MULTI7_S1_Pin|MULTI7_S2_Pin|MULTI7_S3_Pin|MULTI8_S2_Pin
                          |MULTI8_S3_Pin|MULTI2_S1_Pin|MULTI2_S2_Pin|MULTI2_S3_Pin
                          |MULTI3_E_Pin|MULTI4_S1_Pin|MULTI4_S2_Pin|MULTI4_S3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : MULTI8_E_Pin MULTI8_S1_Pin */
  GPIO_InitStruct.Pin = MULTI8_E_Pin|MULTI8_S1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : HX711_DOUT_Pin USER_BTN_Pin */
  GPIO_InitStruct.Pin = HX711_DOUT_Pin|USER_BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : USER_LED_Pin HX711_SCK_Pin MULTI3_S1_Pin MULTI3_S2_Pin
                           MULTI3_S3_Pin MULTI4_E_Pin */
  GPIO_InitStruct.Pin = USER_LED_Pin|HX711_SCK_Pin|MULTI3_S1_Pin|MULTI3_S2_Pin
                          |MULTI3_S3_Pin|MULTI4_E_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : MULTI7_E_Pin MULTI1_E_Pin MULTI1_S1_Pin MULTI1_S2_Pin
                           MULTI1_S3_Pin MULTI2_E_Pin MULTI5_S1_Pin MULTI5_S2_Pin
                           MULTI5_S3_Pin MULTI6_E_Pin MULTI6_S1_Pin MULTI6_S2_Pin
                           MULTI6_S3_Pin */
  GPIO_InitStruct.Pin = MULTI7_E_Pin|MULTI1_E_Pin|MULTI1_S1_Pin|MULTI1_S2_Pin
                          |MULTI1_S3_Pin|MULTI2_E_Pin|MULTI5_S1_Pin|MULTI5_S2_Pin
                          |MULTI5_S3_Pin|MULTI6_E_Pin|MULTI6_S1_Pin|MULTI6_S2_Pin
                          |MULTI6_S3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : MULTI5_E_Pin */
  GPIO_InitStruct.Pin = MULTI5_E_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MULTI5_E_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
