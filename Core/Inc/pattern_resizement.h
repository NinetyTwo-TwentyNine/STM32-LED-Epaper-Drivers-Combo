#ifndef __PATTERNRESIZE_H
#define __PATTERNRESIZE_H

// ================================================================================
// Pattern resizement H-file
// ================================================================================

#include "fake_math.h"
#include "main.h"

uint8_t checkIfArrayHasCoords(int y, int x, int array[][2], int size);

void clearCoordsArray();
void addToCoordsArray(int y, int x);
void addOffsetToVector(uint8_t patternN);
void convertPatternSize(uint8_t array[][2], uint8_t size);

#endif /* __PATTERNRESIZE_H */
