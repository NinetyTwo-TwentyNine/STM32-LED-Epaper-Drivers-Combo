/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
const uint8_t VECTOR_DEFAULT_PATTERNS_SIZES[] = {8, 8, 12, 16, 14, 14};

const uint8_t VECTOR_DEFAULT_PATTERNS[][16][2] = {
    // { {5,1}, {4,0}, {3,1}, {2,0}, {1,1}, {0,0}, {0,1}, {1,0}, {2,1}, {3,0}, {4,1}, {5,0} },
    // { {1,5}, {0,4}, {1,3}, {0,2}, {1,1}, {0,0}, {1,0}, {0,1}, {1,2}, {0,3}, {1,4}, {0,5} },
    { {0,1}, {1,0}, {2,0}, {3,1}, {3,2}, {2,3}, {1,3}, {0,2} },
	{ {0,0}, {1,0}, {2,0}, {1,1}, {0,2}, {1,2}, {2,2}, {1,1} },
    { {0,0}, {0,1}, {0,2}, {0,3}, {1,3}, {2,3}, {3,3}, {3,2}, {3,1}, {3,0}, {2,0}, {1,0} },
    { {3,1}, {4,0}, {5,0}, {6,1}, {6,2}, {5,3}, {4,3}, {3,2}, {3,1}, {2,0}, {1,0}, {0,1}, {0,2}, {1,3}, {2,3}, {3,2} },
    { {0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0}, {6,0}, {7,0}, {6,0}, {5,0}, {4,0}, {3,0}, {2,0}, {1,0} },
    { {0,0}, {0,1}, {0,2}, {0,3}, {0,4}, {0,5}, {0,6}, {0,7}, {0,6}, {0,5}, {0,4}, {0,3}, {0,2}, {0,1} }
};

uint8_t PATTERNS_NOARROW[PATTERN_COUNT + 1] = {false, false, false, false, false, false};
uint8_t PATTERNS_MIRROR[PATTERN_COUNT + 1] = {false, false, false, false, false, false};
uint8_t PATTERNS_MIRROR_AXIS[PATTERN_COUNT + 1] = {0, 0, 0, 0, 0, 0};

uint32_t PATTERN_VECTOR_SIZE = 0;
uint8_t PATTERN_VECTOR_RESIZED[128][2];

uint32_t seg_display_num = 0, last_matrix_update_time = 0, last_display_update_time = 0;
uint8_t matrix_regime = 2, matrix_time_regime = 1, matrix_reset_button_pressed = false, matrix_enabled = true;

const uint32_t SECURITY_CODE_CONDENSED = 470;
uint8_t exercise_datapackage[EXERCISE_DATAPACKAGE_SIZE];
uint8_t rx_index, uart_transaction_cmplt;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
DMA_HandleTypeDef hdma_tim2_ch1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	if (htim->Instance == TIM3) {
		  if (matrix_enabled) {
			  last_matrix_update_time = HAL_GetTick();
			  playPattern(matrix_regime);
		  }
	}
	else if (htim->Instance == TIM4) {
		  showSegmentDisplay(GPIOB, GPIO_PIN_3, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_4);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    last_display_update_time = HAL_GetTick();
    last_matrix_update_time = HAL_GetTick();
    resetMatrixParams();

    seg_display_num = 0;
	updateSegmentBuffers();
	showSegmentDisplay(GPIOB, GPIO_PIN_3, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_4);

	setupNewPattern();

    cleanNewPattern();
	HAL_UART_Receive_IT(&huart1, exercise_datapackage, sizeof(exercise_datapackage));
}

void EXTI2_IRQHandler()
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_2) != 0x00u)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
        //GPIO_PinState state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);

        resetMatrix();
    }
}
void EXTI3_IRQHandler()
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_3) != 0x00u)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
        //GPIO_PinState state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3);

        uint8_t old_time_regime = matrix_time_regime;
    	matrix_time_regime = (matrix_time_regime + 1) % TIME_REGIME_COUNT;
    	brightness_tick *= (matrix_time_regime + 1) / (old_time_regime + 1);
    	//htim3.Init.Prescaler = 30 * (matrix_time_regime + 1) - 1;
    }
}
void EXTI4_IRQHandler()
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_4) != 0x00u)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
        //GPIO_PinState state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);

	    matrix_regime = (matrix_regime == PATTERN_COUNT) ? 0 : ((matrix_regime + 1) % PATTERN_COUNT);
	    setupDefaultPattern();

	    last_display_update_time = HAL_GetTick();
	    last_matrix_update_time = HAL_GetTick();
	    resetMatrixParams();

	    seg_display_num = 0;
		updateSegmentBuffers();
		showSegmentDisplay(GPIOB, GPIO_PIN_3, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_4);
    }
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void setupDefaultPattern()
{
  uint8_t VECTOR_CURR_EXERCISE[16][2];
  uint8_t size = (VECTOR_DEFAULT_PATTERNS_SIZES[matrix_regime]);
//  Serial.println(size);
  for (uint8_t i = 0; i < size; i++)
  {
    VECTOR_CURR_EXERCISE[i][0] = (VECTOR_DEFAULT_PATTERNS[matrix_regime][i][0]);
    VECTOR_CURR_EXERCISE[i][1] = (VECTOR_DEFAULT_PATTERNS[matrix_regime][i][1]);
  }
  convertPatternSize(VECTOR_CURR_EXERCISE, size);
}

void cleanNewPattern()
{
  for (int i = 0; i < EXERCISE_DATAPACKAGE_SIZE; i++)
  {
    exercise_datapackage[i] = 0;
  }
}

void setupNewPattern()
{
  for (int i = 0; i < EXERCISE_SECURITY_CODE_LENGTH; i++) {
    if (exercise_datapackage[i] != exercise_datapackage[EXERCISE_DATAPACKAGE_SIZE - i - 1]) {
      return;
    }
  }

  int exercise_length = exercise_datapackage[EXERCISE_SECURITY_CODE_LENGTH];
  uint8_t VECTOR_NEW_EXERCISE[EXERCISE_PATTERN_DATA_LENGTH][2];
  for (int i = 0; i < exercise_length; i++)
  {
    VECTOR_NEW_EXERCISE[i][0] = exercise_datapackage[i * 2 + EXERCISE_SECURITY_CODE_LENGTH + 1];
    VECTOR_NEW_EXERCISE[i][1] = exercise_datapackage[i * 2 + EXERCISE_SECURITY_CODE_LENGTH + 2];
  }
  PATTERNS_MIRROR[PATTERN_COUNT] = (exercise_datapackage[EXERCISE_DATAPACKAGE_SIZE - EXERCISE_SECURITY_CODE_LENGTH - 2] != 0);
  if (PATTERNS_MIRROR[PATTERN_COUNT]) { PATTERNS_MIRROR_AXIS[PATTERN_COUNT] = exercise_datapackage[EXERCISE_DATAPACKAGE_SIZE - EXERCISE_SECURITY_CODE_LENGTH - 2] % 2; }
  PATTERNS_NOARROW[PATTERN_COUNT] = (exercise_datapackage[EXERCISE_DATAPACKAGE_SIZE - EXERCISE_SECURITY_CODE_LENGTH - 1] != 1);

  convertPatternSize(VECTOR_NEW_EXERCISE, exercise_length);
  matrix_regime = PATTERN_COUNT;
  pattern_tick = 0;
}

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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  //HAL_TIM_Base_Start_IT(&htim3);
  //HAL_TIM_Base_Start_IT(&htim4);
  HAL_UART_Receive_IT(&huart1, exercise_datapackage, sizeof(exercise_datapackage));

  setupDefaultPattern();

  uint32_t display_delay_save = 0, matrix_delay_save = 0;
  updateSegmentBuffers();

  /*
  // while loop:
	  GPIO_PinState state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);
	  uint8_t reset_button_pressed = (state == GPIO_PIN_RESET);
	  if (matrix_reset_button_pressed != reset_button_pressed)
	  {
		  if (reset_button_pressed)
		  {
			  display_delay_save = HAL_GetTick() - last_display_update_time;
			  matrix_delay_save = HAL_GetTick() - last_matrix_update_time;
		  }
		  else
		  {
			  last_display_update_time = HAL_GetTick() - display_delay_save;
			  last_matrix_update_time = HAL_GetTick() - matrix_delay_save;
		  }
		  resetMatrix();
	  }
	  matrix_reset_button_pressed = reset_button_pressed;

	  if ( (HAL_GetTick() - last_matrix_update_time) >= MATRIX_DELAY_TIME ) {
		  if (matrix_enabled) {
			  last_matrix_update_time = HAL_GetTick();
			  playPattern(matrix_regime);
		  }
	  }

	  showSegmentDisplay(GPIOB, GPIO_PIN_3, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_4);

	  if (matrix_enabled) {
		  if ( (HAL_GetTick() - last_display_update_time) >= 1000 )
		  {
			  last_display_update_time = HAL_GetTick();
			  seg_display_num++;
			  updateSegmentBuffers();
		  }
	  }
  */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  GPIO_PinState state = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);
	  uint8_t reset_button_pressed = (state == GPIO_PIN_RESET);
	  if (matrix_reset_button_pressed != reset_button_pressed)
	  {
		  if (reset_button_pressed)
		  {
			  display_delay_save = HAL_GetTick() - last_display_update_time;
			  matrix_delay_save = HAL_GetTick() - last_matrix_update_time;
		  }
		  else
		  {
			  last_display_update_time = HAL_GetTick() - display_delay_save;
			  last_matrix_update_time = HAL_GetTick() - matrix_delay_save;
		  }
		  resetMatrix();
	  }
	  matrix_reset_button_pressed = reset_button_pressed;

	  if ( (HAL_GetTick() - last_matrix_update_time) >= MATRIX_DELAY_TIME ) {
		  if (matrix_enabled) {
			  last_matrix_update_time = HAL_GetTick();
			  playPattern(matrix_regime);
		  }
	  }

	  showSegmentDisplay(GPIOB, GPIO_PIN_3, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_4);

	  if (matrix_enabled) {
		  if ( (HAL_GetTick() - last_display_update_time) >= 1000 )
		  {
			  last_display_update_time = HAL_GetTick();
			  seg_display_num++;
			  updateSegmentBuffers();
		  }
	  }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL15;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 75-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 32-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 30000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 1-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 15000-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  /*Configure GPIO pins : PA10 */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;  // Keep as input mode
  GPIO_InitStruct.Pull = GPIO_PULLUP;      // Enable internal pull-up resistor
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA2 PA3 PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
  /*Configure GPIO pins : PA10 */
//  GPIO_InitStruct.Pin = GPIO_PIN_10;
//  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;  // Keep as input mode
//  GPIO_InitStruct.Pull = GPIO_PULLUP;      // Enable internal pull-up resistor
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  //  NVIC_EnableIRQ(EXTI2_IRQn);
  //  NVIC_SetPriority(EXTI2_IRQn, 2);

    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_SetPriority(EXTI3_IRQn, 2);

    NVIC_EnableIRQ(EXTI4_IRQn);
    NVIC_SetPriority(EXTI4_IRQn, 2);
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
