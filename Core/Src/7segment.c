
// ================================================================================
// 7-segment control functions C-file
// ================================================================================

#include "7segment.h"

uint8_t last_updated_segment = 0, last_segment_update_counter = 0;
int8_t last_segment_direction = 1;

uint8_t digitBuffer[DIGIT_COUNT];

const uint8_t digit[10] = {  // маска для 7 сигментного индикатора
//    0b11000000,
//    0b11111001,
//    0b10100100,
//    0b10110000,
//    0b10011001,
//    0b10010010,
//    0b10000010,
//    0b11111000,
//    0b10000000,
//    0b10010000,
	0b11000000,
	0b11001111,
	0b10100100,
	0b10000110,
	0b10001011,
	0b10010010,
	0b10010000,
	0b11000111,
	0b10000000,
	0b10000010,
};

const uint8_t chr[8] = { // маска для разряда
	0b00000001,
	0b00000010,
	0b00000100,
	0b00001000,
	0b00010000,
	0b00100000,
	0b01000000,
	0b10000000
};

void shiftOut(GPIO_TypeDef * dataPort, uint16_t dataPin, GPIO_TypeDef * clockPort, uint16_t clockPin, uint8_t bitOrder, uint8_t val)
{
	for (uint8_t i = 0; i < 8; i++)  {
		if (bitOrder == 0) {
			HAL_GPIO_WritePin(dataPort, dataPin, (val & 1) ? GPIO_PIN_SET : GPIO_PIN_RESET); //digitalWrite(dataPin, val & 1);
			val >>= 1;
		} else {
			HAL_GPIO_WritePin(dataPort, dataPin, ((val & 128) != 0) ? GPIO_PIN_SET : GPIO_PIN_RESET); //digitalWrite(dataPin, (val & 128) != 0);
			val <<= 1;
		}

		HAL_GPIO_WritePin(clockPort, clockPin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(clockPort, clockPin, GPIO_PIN_RESET);
	}
}

void showSegmentDisplay(GPIO_TypeDef * dataPort, uint16_t dataPin, GPIO_TypeDef * clockPort, uint16_t clockPin, GPIO_TypeDef * latchPort, uint16_t latchPin)
{
  for (uint8_t i = 0; i < DIGIT_COUNT; i++)
  {
	uint8_t j = (last_updated_segment + i) % DIGIT_COUNT;
	uint8_t curr_digit = digit[digitBuffer[j]];
	if (j == 1)
	{
		curr_digit &= ~(1 << 7);
	}

	HAL_GPIO_WritePin(latchPort, latchPin, GPIO_PIN_RESET); // открываем защелку PORTD &= ~(1 << RCLK)
    shiftOut(dataPort, dataPin, clockPort, clockPin, 1, curr_digit);  // отправляем байт с "числом"
    shiftOut(dataPort, dataPin, clockPort, clockPin, 1, chr[j]);   // включаем разряд
	HAL_GPIO_WritePin(latchPort, latchPin, GPIO_PIN_SET); // защелкиваем регистры PORTD |= (1 << RCLK)
  }
  last_segment_update_counter = (last_segment_update_counter + 1) % 160;
  if (last_segment_update_counter == 0)
  {
	//last_updated_segment = (last_updated_segment + 1) % DIGIT_COUNT;
	last_updated_segment += last_segment_direction;
	if (last_updated_segment == 1 || last_updated_segment == DIGIT_COUNT - 2) // WTF
	{
		last_segment_direction *= -1;
	}
  }
}

void updateSegmentBuffers()
{
  uint8_t secs = seg_display_num % 60, mins = seg_display_num / 60;
  for (uint8_t i = 0; i < sizeof(digitBuffer); i++)
  {
	if (i < sizeof(digitBuffer) / 2)
	{
	  digitBuffer[i] = secs % 10;
	  secs /= 10;
	}
	else
	{
	  digitBuffer[i] = mins % 10;
	  mins /= 10;
	}
  }
}
