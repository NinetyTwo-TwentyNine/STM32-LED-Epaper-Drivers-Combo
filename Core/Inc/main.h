/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ws2812.h"
#include "7segment.h"

#include "pattern_resizement.h"
#include "main_matrix_functions.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define false 0
#define true 1


#define LED_SIDE_COUNT 16
#define LED_BUFFER_SIZE 1
#define LED_COUNT (LED_SIDE_COUNT*LED_SIDE_COUNT)
#define PATTERN_COUNT 6
#define TIME_REGIME_COUNT 3
#define MATRIX_DELAY_TIME 12
#define MATRIX_FULL_TICK_TIME (MATRIX_DELAY_TIME * 2)
#define EXERCISE_PATTERN_DATA_LENGTH 32
#define EXERCISE_SECURITY_CODE_LENGTH 10
#define EXERCISE_DATAPACKAGE_SIZE (EXERCISE_SECURITY_CODE_LENGTH*2 + EXERCISE_PATTERN_DATA_LENGTH*2 + 3)
/*
#define BUTTON_SELECT_PIN = 3
#define BUTTON_ITERATE_PIN = 2
#define BUTTON_RESET_PIN = 1
*/

extern const uint8_t VECTOR_DEFAULT_PATTERNS_SIZES[];
extern const uint8_t VECTOR_DEFAULT_PATTERNS[][16][2];

extern uint8_t PATTERNS_NOARROW[];
extern uint8_t PATTERNS_MIRROR[];
extern uint8_t PATTERNS_MIRROR_AXIS[];

extern uint32_t PATTERN_VECTOR_SIZE;
extern uint8_t PATTERN_VECTOR_RESIZED[][2];

extern uint32_t seg_display_num;
extern uint8_t matrix_regime, matrix_time_regime, matrix_reset_button_pressed, matrix_enabled;

extern const uint32_t SECURITY_CODE_CONDENSED;
extern uint8_t exercise_datapackage[];


extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;
/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void setupDefaultPattern();
void cleanNewPattern();
void setupNewPattern();

void showSegmentDisplay(GPIO_TypeDef * dataPort, uint16_t dataPin, GPIO_TypeDef * clockPort, uint16_t clockPin, GPIO_TypeDef * latchPort, uint16_t latchPin);
void shiftOut(GPIO_TypeDef * dataPort, uint16_t dataPin, GPIO_TypeDef * clockPort, uint16_t clockPin, uint8_t bitOrder, uint8_t val);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
