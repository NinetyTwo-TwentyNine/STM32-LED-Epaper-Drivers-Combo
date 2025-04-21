/*
 * epaper.h
 */

#include "main.h"
#include "fake_math.h"

#ifndef INC_EPAPER_H_
#define INC_EPAPER_H_


#define EPAPER_WIDTH 250
#define EPAPER_HEIGHT 122

#define EPAPER_CELL_AMOUNT_Y 16
#define EPAPER_CELL_AMOUNT_X 257 // Not necessarily

#define SCREEN_RESET_SOFT 0
#define BUSY_IMPLEMENT_SIMPLE 1

extern const uint8_t LUTDefault_full[];
extern const uint8_t LUTDefault_full_soft[];

extern uint8_t buffer[];

void preInit();
void init();

void powerOn();
void reset();
void update();
void deepSleep();

void waitWhileBusy();
void waitWhileBusyIT();
void waitWhileBusySimple();

void sendCommand(uint8_t command);
void sendData(uint8_t data);
void writeMemory(uint8_t command, uint8_t* data, uint16_t size);

void sendPatternData(uint8_t data[][2], int size, uint8_t shouldUpdate);


#endif /* INC_EPAPER_H_ */
