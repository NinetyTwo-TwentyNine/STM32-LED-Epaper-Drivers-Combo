#include "epaper.h"


const uint8_t LUTDefault_full[] = {
		0x80,	0x4A,	0x40,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x40,	0x4A,	0x80,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x80,	0x4A,	0x40,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x40,	0x4A,	0x80,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0xF,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0xF,	0x0,	0x0,	0xF,	0x0,	0x0,	0x2,
		0xF,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x1,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x22,	0x22,	0x22,	0x22,	0x22,	0x22,	0x0,	0x0,	0x0,
		0x22,	0x17,	0x41,	0x0,	0x32,	0x36
};

const uint8_t LUTDefault_full_soft[] = {
		0x0,	0x40,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x80,	0x80,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x40,	0x40,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x80,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x14,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x1,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x1,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x0,	0x0,	0x0,	0x0,	0x0,	0x0,	0x0,
		0x22,	0x22,	0x22,	0x22,	0x22,	0x22,	0x0,	0x0,	0x0,
		0x22,	0x17,	0x41,	0x00,	0x32,	0x36,
};

uint8_t buffer[EPAPER_CELL_AMOUNT_X * EPAPER_CELL_AMOUNT_Y] = {}; // Possible size values: 4000, 4112, 4128


void waitWhileBusy()
{
#if (BUSY_IMPLEMENT_SIMPLE)
	{
		waitWhileBusySimple();
	}
#else
	{
		waitWhileBusyIT();
	}
#endif
}

static int busySet;
void waitWhileBusyIT()
{
    GPIO_PinState state = HAL_GPIO_ReadPin(EPD_BUSY_Port, EPD_BUSY_Pin);
    if(state == GPIO_PIN_SET)
    {
        busySet = 0;
        while(!busySet){}
    }
}
void EXTI3_IRQHandler()
{
    if (__HAL_GPIO_EXTI_GET_IT(EPD_BUSY_Pin) != 0x00u)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(EPD_BUSY_Pin);
        busySet = 1;
    }
}

void waitWhileBusySimple()
{
    GPIO_PinState state = HAL_GPIO_ReadPin(EPD_BUSY_Port, EPD_BUSY_Pin);
    while (state == GPIO_PIN_SET) {state = HAL_GPIO_ReadPin(EPD_BUSY_Port, EPD_BUSY_Pin);}
}

void sendCommand(uint8_t command)
{
    HAL_GPIO_WritePin(EPD_CS_Port, EPD_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(EPD_DC_Port, EPD_DC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(EPD_CS_Port, EPD_CS_Pin, GPIO_PIN_SET);
}

void sendData(uint8_t data)
{
    HAL_GPIO_WritePin(EPD_CS_Port, EPD_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(EPD_DC_Port, EPD_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(&hspi2, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(EPD_CS_Port, EPD_CS_Pin, GPIO_PIN_SET);
}

void writeMemory(uint8_t command, uint8_t* data, uint16_t size)
{
    HAL_GPIO_WritePin(EPD_DC_Port, EPD_DC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, &command, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(EPD_DC_Port, EPD_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(&hspi2, data, size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(EPD_CS_Port, EPD_CS_Pin, GPIO_PIN_SET);
}

void powerOn()
{
	//sendCommand(0x12); // 0x04
}

void deepSleep()
{
	sendCommand(0x10);
	sendData(0x01);
}

void update()
{
	sendCommand(0x22);
	sendData(0xC7);
	sendCommand(0x20);
	waitWhileBusy();
}

void reset()
{
    HAL_GPIO_WritePin(EPD_DC_Port, EPD_DC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(EPD_CS_Port, EPD_CS_Pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(EPD_RST_Port, EPD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(EPD_RST_Port, EPD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(EPD_RST_Port, EPD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(20);
}

void preInit()
{
	// Control panel configuration; Gate selection
	sendCommand(0x01);
	sendData((250-1)%256);
	sendData((250-1)/256);
	sendData(0x00);

	// BOOSTER_SOFT_START_CONTROL
	sendCommand(0x0C);
	sendData(0xD7);
	sendData(0xD6);
	sendData(0x9D);

	// VCOM
	sendCommand(0x2C);
	sendData(0xA8);

	// Dummy line per gate (4)
	sendCommand(0x3A);
	sendData(0x1A);
	//sendData(0x06);

	// Gate time setting (2us per line)
	sendCommand(0x3B);
	sendData(0x08);
	//sendData(0x0B);

    sendCommand(0x3C);	// BORDER_WAVEFORM_CONTROL
    sendData(0x03);

    //  Display update control
    sendCommand(0x21);
    sendData(0x00);
    sendData(0x80);

	// Entry sequence (0x00)
	sendCommand(0x11);
	sendData(0x01);

	// RAMX Start/end position
	sendCommand(0x44);
	sendData(0x00);
	sendData(0x0F); // Default = 0x0F; 0x7A for full (not 1/8)

	// RAMY Start/end position
	sendCommand(0x45);
	sendData(0x00);
	sendData(0xF9); // Default = 0xF9
	sendData(0x00);
	sendData(0x00);
}

void init()
{
	reset();
	waitWhileBusy();

	sendCommand(0x12);
	waitWhileBusy();

	preInit();
	waitWhileBusy();

	sendCommand(0x32);

	const uint8_t* LUTDefault_chosen;
#if (SCREEN_RESET_SOFT)
	LUTDefault_chosen = LUTDefault_full_soft;
#else
	LUTDefault_chosen = LUTDefault_full;
#endif

	for (int i = 0; i < 159; i++) sendData(*(LUTDefault_chosen + i));

	sendCommand(0x3f);
	sendData(*(LUTDefault_chosen+153));
	sendCommand(0x03);	// gate voltage
	sendData(*(LUTDefault_chosen+154));
	sendCommand(0x04);	// source voltage
	sendData(*(LUTDefault_chosen+155));	// VSH
	sendData(*(LUTDefault_chosen+156));	// VSH2
	sendData(*(LUTDefault_chosen+157));	// VSL
	sendCommand(0x2c);		// VCOM
	sendData(*(LUTDefault_chosen+158));

	powerOn();
}

void sendPatternData(uint8_t data[][2], int size, uint8_t shouldUpdate)
{
  // RAMX address counter
  sendCommand(0x4E);
  sendData(0x00); // Default = 0x07

  // RAMY address counter
  sendCommand(0x4F);
  sendData(0x00); // Default = 0x7C (to start in the center)
  sendData(0x00);

    for (int i = 0; i < sizeof(buffer); i++) {
      buffer[i] = 0x00;
    }

//    uint8_t uart1_data[] = "hello from uart.\n\r";
//    HAL_UART_Transmit(&huart1, uart1_data, sizeof(uart1_data), 50);
//
//    uint8_t uart_buffer[50];
//    sprintf(uart_buffer, "y = %d.\n\r", data[2][0]);
//    HAL_UART_Transmit(&huart1, uart_buffer, 10, 50);


    for (int i = 0; i < size; i++)
    {
      uint32_t point_cell_x = math_round((float)data[i][1] * (EPAPER_CELL_AMOUNT_X - 1) / (EPAPER_WIDTH - 1));
      uint32_t point_width = math_round((float)(data[i][1]+1) * (EPAPER_CELL_AMOUNT_X - 1) / (EPAPER_WIDTH - 1)) - point_cell_x;
      uint32_t point_cell_y_8 = math_round((float)data[i][0] * (EPAPER_CELL_AMOUNT_Y - 1) * 8 / (EPAPER_HEIGHT - 1));
      uint32_t point_height = math_round((float)(data[i][0]+1) * (EPAPER_CELL_AMOUNT_Y - 1) * 8 / (EPAPER_HEIGHT - 1)) - point_cell_y_8;

      for (int j = 1; j <= point_height; j++)
      {
        uint32_t curr_cell_x = point_cell_x, curr_cell_y = point_cell_y_8 / 8;
        if (8 - (point_cell_y_8 % 8) < j)
        {
          curr_cell_y++;
        }

        for (int k = 0; k < point_width; k++)
        {
            uint32_t index = curr_cell_x * EPAPER_CELL_AMOUNT_Y + curr_cell_y;
            buffer[index] |= 1 << (8 - (point_cell_y_8 % 8 + j - 1) % 8 - 1);
            curr_cell_x++;
        }
      }
    }

    sendCommand(0x24);
    for (int i = 0; i < sizeof(buffer); i++)
    {
      sendData(buffer[i]);
    }

    if (shouldUpdate)
    {
        waitWhileBusy();
        update();
    }
}

/*
void updateDisplay()
{
	setRamPointer(0x00, (yDot-1)%256, (yDot-1)/256);
	sendDisplayRam(xDot, yDot, (uint8_t*)displayMemory);
	update();
}
*/
