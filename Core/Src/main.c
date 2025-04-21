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
uint8_t PATTERN_VECTOR_RESIZED[1024][2];

uint32_t last_update_time = 0;
uint8_t matrix_regime = 2, matrix_reset_button_pressed = false, matrix_enabled = true;

const uint32_t SECURITY_CODE_CONDENSED = 470;
uint8_t exercise_datapackage[EXERCISE_DATAPACKAGE_SIZE];
uint8_t backup_datapackage[EXERCISE_DATAPACKAGE_SIZE];
uint8_t rx_index, uart_transaction_cmplt;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
//	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    last_update_time = HAL_GetTick();
    waitWhileBusy();

    NVIC_DisableIRQ(EXTI4_IRQn);
    NVIC_DisableIRQ(EXTI9_5_IRQn);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    setupNewPattern();
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    NVIC_EnableIRQ(EXTI4_IRQn);
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    cleanNewPattern();
	HAL_UART_Receive_IT(&huart1, exercise_datapackage, sizeof(exercise_datapackage));
}

void EXTI4_IRQHandler()
{
        if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_4) != 0x00u)
    	{
    	    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);

    	    if ( (last_update_time - HAL_GetTick()) < 300 )
    	    {
    	    	return;
    	    }
    	    last_update_time = HAL_GetTick();

    	    //matrix_regime = (matrix_regime - 1) % PATTERN_COUNT;
    	    matrix_regime = (matrix_regime == 0) ? (PATTERN_COUNT - 1) : (matrix_regime - 1);

    	    NVIC_DisableIRQ(EXTI4_IRQn);
    	    NVIC_DisableIRQ(EXTI9_5_IRQn);
    	    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    	    setupDefaultPattern();
    	    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    	    NVIC_EnableIRQ(EXTI4_IRQn);
    	    NVIC_EnableIRQ(EXTI9_5_IRQn);
    	}
}

void EXTI9_5_IRQHandler()
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != 0x00u)
	{
	    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);

	    if ( (last_update_time - HAL_GetTick()) < 300 )
	    {
	    	return;
	    }
	    last_update_time = HAL_GetTick();

	    sendCurrentPattern();
	}
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_6) != 0x00u)
	{
	    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);

	    if ( (last_update_time - HAL_GetTick()) < 300 )
	    {
	    	return;
	    }
	    last_update_time = HAL_GetTick();

	    matrix_regime = (matrix_regime == PATTERN_COUNT) ? 0 : ((matrix_regime + 1) % PATTERN_COUNT);

	    NVIC_DisableIRQ(EXTI4_IRQn);
	    NVIC_DisableIRQ(EXTI9_5_IRQn);
	    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
	    setupDefaultPattern();
	    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	    NVIC_EnableIRQ(EXTI4_IRQn);
	    NVIC_EnableIRQ(EXTI9_5_IRQn);
	}
}


void Display_GPIO_Init()
{
    HAL_GPIO_WritePin(EPD_RST_Port, EPD_RST_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(EPD_CS_Port, EPD_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(EPD_DC_Port, EPD_DC_Pin, GPIO_PIN_RESET);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = EPD_RST_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(EPD_RST_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = EPD_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(EPD_CS_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = EPD_DC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(EPD_DC_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = EPD_BUSY_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(EPD_BUSY_Port, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(EPD_BUSY_EXTI_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EPD_BUSY_EXTI_IRQn);
}

void displayCurrentPattern()
{
	  if (PATTERNS_MIRROR[matrix_regime])
	  {
		  uint8_t VECTOR_CURR_EXERCISE_RESIZED[2048][2];
		  for (int i = 0; i < PATTERN_VECTOR_SIZE; i++)
		  {
			  VECTOR_CURR_EXERCISE_RESIZED[i][0] = PATTERN_VECTOR_RESIZED[i][0];
			  VECTOR_CURR_EXERCISE_RESIZED[i][1] = PATTERN_VECTOR_RESIZED[i][1];

			  VECTOR_CURR_EXERCISE_RESIZED[PATTERN_VECTOR_SIZE + i][0] = PATTERNS_MIRROR_AXIS[matrix_regime] ? (CELL_SIDE_COUNT_Y - PATTERN_VECTOR_RESIZED[i][0] + 1) : PATTERN_VECTOR_RESIZED[i][0];
			  VECTOR_CURR_EXERCISE_RESIZED[PATTERN_VECTOR_SIZE + i][1] = PATTERNS_MIRROR_AXIS[matrix_regime] ? PATTERN_VECTOR_RESIZED[i][1] : (CELL_SIDE_COUNT_X - PATTERN_VECTOR_RESIZED[i][1] + 1);
		  }
		  sendPatternData(VECTOR_CURR_EXERCISE_RESIZED, PATTERN_VECTOR_SIZE*2, true);
	  }
	  else
	  {
		  sendPatternData(PATTERN_VECTOR_RESIZED, PATTERN_VECTOR_SIZE, true);
	  }
}

void setupDefaultPattern()
{
  uint8_t VECTOR_CURR_EXERCISE[16][2];
  uint8_t size = (VECTOR_DEFAULT_PATTERNS_SIZES[matrix_regime]);
//  Serial.println(size);
  for (int i = 0; i < size; i++)
  {
    VECTOR_CURR_EXERCISE[i][0] = (VECTOR_DEFAULT_PATTERNS[matrix_regime][i][0]);
    VECTOR_CURR_EXERCISE[i][1] = (VECTOR_DEFAULT_PATTERNS[matrix_regime][i][1]);
  }
  convertPatternSize(VECTOR_CURR_EXERCISE, size);

  displayCurrentPattern();
}

void cleanNewPattern()
{
  for (int i = 0; i < EXERCISE_DATAPACKAGE_SIZE; i++)
  {
	backup_datapackage[i] = 0;
  }
  for (int i = 0; i < EXERCISE_DATAPACKAGE_SIZE; i++)
  {
	backup_datapackage[i] = exercise_datapackage[i];
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

  displayCurrentPattern();
}

void sendCurrentPattern()
{
	if (matrix_regime < PATTERN_COUNT)
	{
		int securityCodeDuplicate = SECURITY_CODE_CONDENSED;
		for (int i = 0; i < EXERCISE_SECURITY_CODE_LENGTH; i++)
		{
			exercise_datapackage[i] = securityCodeDuplicate % 2;
			securityCodeDuplicate /= 2;
		}
		exercise_datapackage[EXERCISE_SECURITY_CODE_LENGTH] = VECTOR_DEFAULT_PATTERNS_SIZES[matrix_regime];
		for (int i = 0; i < EXERCISE_PATTERN_DATA_LENGTH; i++)
		{
			if (VECTOR_DEFAULT_PATTERNS_SIZES[matrix_regime] > i)
			{
				exercise_datapackage[EXERCISE_SECURITY_CODE_LENGTH + i * 2 + 1] = VECTOR_DEFAULT_PATTERNS[matrix_regime][i][0];
				exercise_datapackage[EXERCISE_SECURITY_CODE_LENGTH + i * 2 + 2] = VECTOR_DEFAULT_PATTERNS[matrix_regime][i][1];
			}
		}
		exercise_datapackage[EXERCISE_DATAPACKAGE_SIZE - EXERCISE_SECURITY_CODE_LENGTH - 2] = PATTERNS_MIRROR[matrix_regime];
		exercise_datapackage[EXERCISE_DATAPACKAGE_SIZE - EXERCISE_SECURITY_CODE_LENGTH - 1] = !PATTERNS_NOARROW[matrix_regime];
		securityCodeDuplicate = SECURITY_CODE_CONDENSED;
		for (int i = 0; i < EXERCISE_SECURITY_CODE_LENGTH; i++)
		{
			exercise_datapackage[EXERCISE_DATAPACKAGE_SIZE - i - 1] = securityCodeDuplicate % 2;
	        securityCodeDuplicate /= 2;
		}

		HAL_UART_Transmit(&huart3, exercise_datapackage, EXERCISE_DATAPACKAGE_SIZE, 50);
	}
	else
	{
		HAL_UART_Transmit(&huart3, backup_datapackage, EXERCISE_DATAPACKAGE_SIZE, 50);
	}
}

//void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
//{
//    GPIO_InitTypeDef GPIO_InitStruct = {0};
//    if(hspi->Instance==SPI2)
//    {
//        __HAL_RCC_SPI2_CLK_ENABLE();
//        __HAL_RCC_GPIOB_CLK_ENABLE();
//        GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15;
//        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//        //GPIO_InitStruct.Alternate = GPIO_AF2_LPTIM;
//        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//    }
//}
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
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */

  //HAL_TIM_Base_Start_IT(&htim2);

  HAL_UART_Receive_IT(&huart1, exercise_datapackage, sizeof(exercise_datapackage));

  Display_GPIO_Init();
  HAL_Delay(10);

  init();
  waitWhileBusy();
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
  setupDefaultPattern();
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
  NVIC_EnableIRQ(EXTI4_IRQn);
  NVIC_EnableIRQ(EXTI9_5_IRQn);
//  deepSleep();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

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

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 200-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 30000-1;
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
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */

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
