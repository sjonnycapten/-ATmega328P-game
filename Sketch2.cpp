#include "Serial.h"
#include "Bullet.h"
#include "Tank.h"
#include "Timer.h"
#include "test.h"
#include "ArduinoNunchuk.h"
#include "MI0283QT9.h"
#include "Grid.h"
#include <Arduino.h>
#include "Screens.h"
#include "Bullet.h"
#include "IR.h"

#define DATA_LENGTH 30
#define RECEIVER_VALUE 38

volatile uint8_t inputChange;
volatile uint16_t updateScreen;
volatile uint16_t counterSend;
volatile uint16_t counterReceive;

char mainBufferSend[DATA_LENGTH];
char inverseBufferSend[DATA_LENGTH];
char mainBufferReceive[DATA_LENGTH];
char inverseBufferReceive[DATA_LENGTH];


ISR(TIMER1_OVF_vect) {
	updateScreen++;
	counterSend++;
	counterReceive++;
}

ISR(INT0_vect) {
	inputChange++;
}


int main(void)
{
	init();
	Serial.begin(9600);

	SerialClass serial(2, 3);					//Setup serial connection

	Timer t1;
	t1.init(RECEIVER_VALUE);
	IRClass ir;
	ir.init(&mainBufferSend[0], &inverseBufferSend[0], &mainBufferReceive[0], &inverseBufferReceive[0], &counterSend, &counterReceive, DATA_LENGTH);
	MI0283QT9 lcd;
	GridClass grid;

	TankClass tank1;
	TankClass tank2;
	tank1.init(&lcd, 1, &serial);
	tank2.init(&lcd, 2, &serial);
	BulletClass bullet1;
	BulletClass bullet2;
	bullet1.init(&lcd);
	bullet2.init(&lcd);
	ScreensClass screens;
	lcd.begin();
	lcd.touchStartCal(); // calibratie wordt gestart
	grid.init(320, 230, 20, &lcd);
	screens.init(&lcd, &serial, &grid);
	uint8_t player = 0, connected = 0;
	screens.setStartPressed(0);

	while (1) { // main loop
		if (!screens.getStartPressed()) { // runs at start of program
			screens.setMenu(connected); // sets the main screen
		}

		if (!connected) {
			player = serial.init(); // init serial and get player
			connected = 1;
		}

		if (screens.getStartInit()) { // if start is clicked or receiver start is pressed
			screens.startGame(&tank1, &tank2); // initializes grid and tanks
		}

		if (updateScreen) { // als het scherm wordt geupdate
			serial.read();
			if (player == 1) {
				screens.setGameScreen(&tank1, &tank2, &bullet1, &bullet2);
			}
			else if(player == 2){
				screens.setGameScreen(&tank2, &tank1, &bullet2,  &bullet1);
			}

			
			if (screens.gameBackPressed()) { // als er op terug wordt geklikt in het spel
				screens.setStartPressed(0); // zet het startscherm neer
			}
		}

		
	}
}
