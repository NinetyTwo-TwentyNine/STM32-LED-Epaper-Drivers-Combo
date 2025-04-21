#ifndef __MATRIXFUNC_H
#define __MATRIXFUNC_H

// ================================================================================
// Main matrix functions H-file
// ================================================================================

#include "main.h"
#include "ws2812.h"

extern uint32_t pattern_tick;
extern uint8_t brightness_tick, pattern_tick_updated;

extern uint8_t PATTERN_CURR_WAYPOINTS_AMOUNT, PATTERN_OLD_WAYPOINTS_AMOUNT;
extern uint8_t PATTERN_CURR_WAYPOINTS[8], PATTERN_OLD_WAYPOINTS[8];

int getPosThroughCoords(int pos_y, int pos_x);
void addCurrPoint(int pos_y, int pos_x);

void resetMatrix();
void resetMatrixParams();
void playPattern(int patternN);

#endif /* __MATRIXFUNC_H */
