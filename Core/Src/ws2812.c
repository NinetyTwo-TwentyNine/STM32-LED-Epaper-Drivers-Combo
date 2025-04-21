
// ================================================================================
// WS2812 LED functions C-file
// ================================================================================

#include "ws2812.h"


int counter = 0;

uint8_t LED_Data[LED_COUNT][3];

void Set_LED(int LEDNum, int Red, int Green, int Blue, int Brightness)
{
	LED_Data[LEDNum][0] = Green;
	LED_Data[LEDNum][1] = Red;
	LED_Data[LEDNum][2] = Blue;

	Set_Brightness(LEDNum, Brightness);
}

void Set_Brightness(int LEDNum, int brightness)  // 0-45
{
#if USE_BRIGHTNESS
	if (brightness   > 100) brightness = 100;
	if (brightness < 0) brightness = 0;
	for (int i = 0; i < 3; i++)
	{
//		float angle = 90-brightness;  // in degrees
//		angle = angle*PI / 180;  // in rad
		LED_Data[LEDNum][i] = brightness * LED_Data[LEDNum][i] / 100; // LED_Data[LEDNum][i])/(tan(angle));
	}

#endif

}

uint16_t pwmData[(24*LED_COUNT)+50];

void WS2812_Send(void)
{
	uint32_t indx=0;
	uint32_t color;


	for (int i = 0; i < LED_COUNT; i++)
	{
		color = ((LED_Data[i][0]<<16) | (LED_Data[i][1]<<8) | (LED_Data[i][2]));

		for (int i=23; i>=0; i--)
		{
			if (color&(1<<i))
			{
				pwmData[indx] = (htim2.Init.Period + 1) * 2 / 3;  // 2/3 of the period divider
			}
			else
			{
				pwmData[indx] = (htim2.Init.Period + 1) * 1 / 3;  // 1/3 of the period divider
			}
			indx++;
		}
	}

	for (int i=0; i<50; i++)
	{
		pwmData[indx] = 0;
		indx++;
	}

	//TIM2->CCR1 = pwmData[0];
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)pwmData, indx);
	//HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);
	//HAL_TIM_Base_Start_IT(&htim3);

//	while (counter < (24*MAX_LED + 50 - 2)) {  HAL_UART_Transmit(&huart1,uart1_data, sizeof(uart1_data), 50);}
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
//	counter++;
//	TIM2->CCR1 = pwmData[counter];
//	HAL_UART_Transmit(&huart1,uart1_data2, sizeof(uart1_data2), 50);
//
//	if ( counter >= 24*MAX_LED + 50 )
//	{
//		HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_1);
//		HAL_UART_Transmit(&huart1,uart1_data3, sizeof(uart1_data3), 50);
//		counter = 0;
//		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
//		return;
//	}

	//HAL_TIM_PWM_Start_IT(&htim2, TIM_CHANNEL_1);

	HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}
