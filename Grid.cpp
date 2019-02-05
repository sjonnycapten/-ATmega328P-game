#include "Grid.h"

#define backgroundColor RGB(212,212,212)

void GridClass::init(uint8_t vXMax, uint8_t vYMax, uint8_t gSize, MI0283QT9 * lcd) {
	this->lcd = new MI0283QT9;
	this->lcd = lcd;
	gridSize = gSize;
	fieldMaxXG = vXMax / gridSize;
	fieldMaxYG = vYMax / gridSize;
}

void GridClass::drawBlock(uint8_t x, uint8_t y, uint8_t color) {			//Draw a square at the given position in the given color
	lcd->fillRect(x*gridSize, y*gridSize, gridSize, gridSize, color);
}

void GridClass::drawBackground() {
	uint8_t x;
	uint8_t y;
	for (y = 0; y <= fieldMaxYG; y++) {				//For every block on the grid, the background will be drawed
		for (x = 0; x < fieldMaxXG; x++) {
			drawBlock(x, y, backgroundColor);
		}
	}
}

GridClass Grid;