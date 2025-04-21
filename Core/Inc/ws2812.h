#ifndef WS2812_H_
#define WS2812_H_

// ================================================================================
// WS2812 LED functions H-file
// ================================================================================

#include "main.h"
//#include "math.h"


#define USE_BRIGHTNESS 1

#define PI 3.14159265


extern uint8_t LED_Data[][3];

void Set_LED(int LEDNum, int Red, int Green, int Blue, int Brightness);
void Set_Brightness(int LEDNum, int brightness);
void WS2812_Send(void);

#endif /* WS2812_H_ */
