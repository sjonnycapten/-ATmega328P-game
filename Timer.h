#ifndef _TIMER_h
#define _TIMER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Timer {
protected:

public:
	void init(uint8_t khz);		//Initialize timers
	void updateBrightness();	//Update brightness of screen
};

#endif