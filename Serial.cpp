// 
// 
// 

#include "Serial.h"

SerialClass::SerialClass(int in, int out) : serial(in, out) {
}

int SerialClass::init()
{
	serial.begin(9600);	// set baudrate on 9600

	serial.listen(); // listen for incomming bytes
	int connection = 0; // init connection
	int player;	// init player

	while (!connection) {
		_delay_ms(50);
		if (serial.available()) { // if has received bytes
			if (serial.peek() == 20) { // if received an message before starting writing set player 2
				player = 2;
				Serial.println("player 2");
				Serial.println("connected");
				serial.write(10);
				serial.flush();

				connection = 1; // go out the while loop
			}
			else if (serial.peek() == 10) { // if received an 10 from other player, set player 1
				player = 1;
				Serial.println("player 1");
				Serial.println("connected");
				serial.write(20);
				serial.flush();
				connection = 1; // go out the while loop
			}
			else {
				Serial.println(serial.peek());
				serial.read();
			}


		}
		else {
			Serial.println("send init");
			serial.write(20); // if nothing received write 20 to other player
		}
		serial.flush();

	}
	return player;
}

SoftwareSerial SerialClass::getSerial() {
	return serial;
}

void SerialClass::send(int message, int var) {
	serial.write(var);
	if (var == 2) {
		uint8_t partOne = message;
		uint8_t partTwo = (message >> 8);
		serial.write(partOne);
		serial.write(partTwo);
	}
	else {
		serial.write(message);
	}
}

uint8_t SerialClass::getShootBullet() {
	uint8_t bullet = shootBullet;
	shootBullet = 0;
	return bullet;
}

uint8_t SerialClass::getPause() {
	uint8_t p = pause;
	pause = 0;
	return p;
}

uint8_t SerialClass::getStart() {
	uint8_t s = start;
	start = 0;
	return s;
}

void SerialClass::read() {
	while (serial.available() > 0) {
		uint8_t var = serial.read();
		uint16_t message = serial.read();
		uint16_t partTwo;
		switch (var) {
			case 1:
				settings = message;
				break;
			case 2:
				xPosTank = message;
				partTwo = serial.read();
				xPosTank |= (partTwo << 8);
				break;
			case 3:
				yPosTank = message;
				break;
			case 4:
				shootBullet = message;
				break;
			case 5:
				pause = message;
				break;
			case 6:
				start = message;
				break;
		}
	}
}

