#include "MI0283QT9.h"

#ifndef _GRID_h
#define _GRID_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class GridClass {
protected:
	MI0283QT9 * lcd;												//Pointer for LCD
	int fieldMaxXG;													//Max X positions of grid
	int fieldMaxYG;													//Max Y positions of grid
	int gridSize;													//Size of grid
public:
	void init(uint8_t vXMax, uint8_t vYMax, uint8_t gSize, MI0283QT9 * lcd);	//Initialize grid components
	void drawBackground();														//Draw background of screen
	void drawBlock(uint8_t x, uint8_t y, uint8_t color);						//Draw block on screen
};

extern GridClass Grid;

#endif