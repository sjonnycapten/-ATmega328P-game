#include <Arduino.h>
#include "Timer.h"
#include "IR.h"
#include "MI0283QT9.h"
#include "ArduinoNunchuk.h"
#include "Tank.h"
#include "Grid.h"
#include "Screens.h"

#define PLAYER1
#define DATA_LENGTH 20

#ifdef PLAYER1
#define PLAYER 1
#define RECEIVER_VALUE 56
#endif

#ifdef PLAYER2
#define PLAYER 2
#define RECEIVER_VALUE 38
#endif

volatile uint8_t inputChange;
volatile uint16_t updateScreen;
volatile uint16_t counterSend;
volatile uint16_t counterReceive;
volatile uint16_t connectionTime;

char mainBufferSend[DATA_LENGTH];
char inverseBufferSend[DATA_LENGTH];
char mainBufferReceive[DATA_LENGTH];
char inverseBufferReceive[DATA_LENGTH];

ISR(TIMER1_OVF_vect) {
	updateScreen++;
	counterSend++;
	counterReceive++;
	connectionTime++;
}

ISR(INT0_vect) {
	inputChange++;
}


int main(void)
{
	init();
	Serial.begin(9600);
	ArduinoNunchuk nunchuk;
	nunchuk.init();
	MI0283QT9 lcd;
	IRClass ir;
	Timer t;
	TankClass tankLeft;
	TankClass tankRight;
	GridClass grid;
	ScreensClass screen;
	t.init(RECEIVER_VALUE);
	ir.init(&mainBufferSend[0], &inverseBufferSend[0], &mainBufferReceive[0], &inverseBufferReceive[0], &tankLeft, &tankRight, &screen, &counterSend, &counterReceive, &connectionTime, &inputChange, PLAYER);
	lcd.begin();
	lcd.touchStartCal();
	grid.init(320, 230, 20, &lcd);
	screen.init(&lcd, &grid, &tankLeft, &tankRight);
	screen.setStartPressedInt(0);
	tankLeft.init(&lcd, 1, &grid, &nunchuk, &tankRight);
	tankRight.init(&lcd, 2, &grid, &nunchuk, &tankLeft);
	if (PLAYER == 1) {
		tankLeft.setPlayer(1);
		tankRight.setPlayer(2);
	}
	else if (PLAYER == 2) {
		tankLeft.setPlayer(2);
		tankRight.setPlayer(1);
	}

	while (1)
	{
		if (!screen.getStartPressedInt()) {		//Runs at start of program
			screen.setMenu(1);					//Sets the main screen
		}
		ir.run();
		t.updateBrightness();
		if (screen.getStartPressedInt()) {
			if (screen.getStartInit()) {		//If start is clicked or receiver start is pressed
				screen.startGame();				//Initializes grid and tanks
			}
			if (ir.getRefreshGame()) {
				screen.refreshGame();
				ir.setRefreshGame(0);
			}

			if (screen.gameBackPressed()) {		//Als er op terug wordt geklikt in het spel
				screen.setStartPressedInt(0);	//Zet het startscherm neer
				screen.setFirstRun(1);
			}
		}
		updateScreen = 0;
	}
}