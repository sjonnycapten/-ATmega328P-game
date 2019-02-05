// Serial.h

#ifndef _SERIAL_h
#define _SERIAL_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <avr/io.h>
#include <SoftwareSerial.h>

class SerialClass
{
protected:


public:
	SerialClass(int in, int out);
	int init();
	SoftwareSerial getSerial();

	void send(int message, int var);


	uint8_t getShootBullet();

	uint8_t getPause();

	uint8_t getStart();

	void read();
	
	uint8_t settings;
	uint16_t xPosTank;
	uint8_t yPosTank;
	uint8_t shootBullet;
	uint8_t pause;
	uint8_t start;
private:
	SoftwareSerial serial;
};

#endif

