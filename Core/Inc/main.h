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
#include "pattern_resizement.h"

#include "epaper.h"
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


#define CELL_SIDE_COUNT_Y 122
#define CELL_SIDE_COUNT_X 250
#define CELL_BUFFER_SIZE 20
#define CELL_COUNT (CELL_SIDE_COUNT_X*CELL_SIDE_COUNT_Y)
#define PATTERN_COUNT 6
#define EXERCISE_SECURITY_CODE_LENGTH 10
#define EXERCISE_PATTERN_DATA_LENGTH 32
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

extern uint32_t common_time;
extern uint8_t matrix_regime, matrix_reset_button_pressed, matrix_enabled;

extern const uint32_t SECURITY_CODE_CONDENSED;
extern uint8_t exercise_datapackage[];
extern uint8_t backup_datapackage[];


#define EPD_RST_Pin GPIO_PIN_1
#define EPD_RST_Port GPIOA
#define EPD_CS_Pin GPIO_PIN_12
#define EPD_CS_Port GPIOB
#define EPD_DC_Pin GPIO_PIN_2
#define EPD_DC_Port GPIOA
#define EPD_BUSY_Pin GPIO_PIN_3
#define EPD_BUSY_Port GPIOA
#define EPD_BUSY_EXTI_IRQn EXTI3_IRQn

extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void displayCurrentPattern();
void setupDefaultPattern();
void cleanNewPattern();
void setupNewPattern();
void sendCurrentPattern();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
