#ifndef __7SEGMENT_H
#define __7SEGMENT_H

// ================================================================================
// 7-segment control functions H-File
// ================================================================================

#include "main.h"

#define DIGIT_COUNT 4

extern uint8_t last_updated_segment, last_segment_update_counter;
extern uint8_t digitBuffer[DIGIT_COUNT];
extern int8_t last_segment_direction;

extern const uint8_t digit[10];
extern const uint8_t chr[8];

void shiftOut(GPIO_TypeDef * dataPort, uint16_t dataPin, GPIO_TypeDef * clockPort, uint16_t clockPin, uint8_t bitOrder, uint8_t val);
void showSegmentDisplay(GPIO_TypeDef * dataPort, uint16_t dataPin, GPIO_TypeDef * clockPort, uint16_t clockPin, GPIO_TypeDef * latchPort, uint16_t latchPin);
void updateSegmentBuffers();

#endif /* __7SEGMENT_H */
