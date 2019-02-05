#include "Screens.h"

#define backgroundColor RGB(212,212,212) // white-ish
#define textColor RGB(13, 71, 161) // dark-blue

void ScreensClass::init(MI0283QT9 * lcdScreen, GridClass * grid, TankClass * tank1, TankClass * tank2) { // Initializes objects and resets variables
	this->lcdScreen = new MI0283QT9;
	this->lcdScreen = lcdScreen;
	this->grid = grid;
	this->tank1 = tank1;
	this->tank2 = tank2;
	scoreCount1 = 0;
	scoreCount2 = 0;
	firstRun = 1;
	playerWin = 0;
}

void ScreensClass::setMenu(uint8_t connected) {	// Sets the menu screen
	if (firstRun) {	// Runs once everytime the Menu is opened
		drawMenuBackground();
		drawMenuText();
		setStartPressedInt(0);
		setHighscoresPressedInt(0);
		firstRun = 0;	// Sets firstRun to zero so that this loop will only run once
	}
	if (!getStartPressedInt()) {	// When start has not been pressed
		if (highscoresPressed()) {	// If highscores has been pressed this if will run once
			drawHighscoresList(); 
			drawHighscoresScreen();
			while (getHighscoresPressedInt()) {	// After highscores has been pressed, this while loop will run
				if (highscoresBackPressed()) {	// If the back button is pressed
					removeHighscoresScreen();
					setHighscoresPressedInt(0);	// Sets is so that highscores has not been pressed, code breaks out of while loop
				}
			}
		}
		startPressed();	// Checks if start has been pressed
	}

	if(getStartPressedInt()) {	// If start has been pressed
		setStartInit(1);	// Initializes the start of the game
		setStartPressedInt(1);	// Start has been pressed
	}

}

void ScreensClass::startGame() {	// Starts the game
	grid->drawBackground();
	setGameBackButton();
	tank1->resetTank();	// Resets the values of both tanks and bullets
	tank2->resetTank();
	setStartInit(0);	// De-initializes the start of the game so that it will once happen once
}

void ScreensClass::refreshGame() {	// Refreshes the game
	if (!playerWin) {	// If the game hasn't ended
		if (tank1->pauseIsHit() || tank2->pauseIsHit()) {	// If the tank or bullet hits the pause button
			setGameBackButton();	// Sets the pause button
		}
		tank1->moveTank();	// Move both tanks
		tank2->moveTank();

		if (tank1->getBulletFired() && tank1->isHit()) {	// If tank 2 is hit by tank 1 
			tank1->resetBullet();	// Removes the bullet of tank 1
			tank2->lowerHealth();	// Lowers the health of tank 2
		}
		if (tank2->getBulletFired() && tank2->isHit()) {	// If tank 1 is hit by tank 2
			tank2->resetBullet();	// Removes the bullet of tank 2
			tank1->lowerHealth();	// Lowers the health of tank 1
		}

		drawHealthText(tank1, tank2);	// Draws the health of both tanks to the screen

		if (tank1->getHealth() == 0) {	// If tank 1's health is 0
			drawWinScreen(tank2->getSide());	// Sets winscreen for tank 2
			playerWin = 1;
		}

		if (tank2->getHealth() == 0) {	// If tank 2's health is 0
			drawWinScreen(tank1->getSide());	// Sets winscreen for tank 1
			playerWin = 1;
		}
	}
	
	if (playerWin) { // If game has ended
		if (screenPressed()) {	// Waiting on screen press
			playerWin = 0; // No one has won
			setStartPressedInt(0); // Start hasn't been pressed
			firstRun = 1;
			setMenu(1);	// Sets the menu screen
		}
	}
}


void ScreensClass::drawMenuBackground() {	// Sets the menu screen's background by filling the screen with a color
	lcdScreen->fillScreen(backgroundColor);
}

void ScreensClass::drawMenuText() {	// Sets the menu screen's text by drawing it to the screen
	lcdScreen->drawText(105, 75, "Start", textColor, backgroundColor, 3);
	lcdScreen->drawText(55, 125, "Highscores", textColor, backgroundColor, 3);
}

void ScreensClass::setGameBackButton() {	// Sets the 'pause' button on the game's screen by filling a cirlce and filling two rectangles inside there
		lcdScreen->fillCircle(160, 16, 11, textColor);
		lcdScreen->fillRect(154, 10, 4, 12, backgroundColor);
		lcdScreen->fillRect(163, 10, 4, 12, backgroundColor);
}

void ScreensClass::drawHighscoresScreen() {	// Removes the menu screen's text and draws the highscore screen's text and button
	lcdScreen->drawText(105, 75, "Start", backgroundColor, backgroundColor, 3);
	lcdScreen->drawText(55, 125, "Highscores", backgroundColor, backgroundColor, 3);
	drawHighscoresText();
	drawHighscoresBackButton();
}

void ScreensClass::removeHighscoresScreen() {	// Removes the highscores screen and draws the menu screen's text
	lcdScreen->drawText(50, 20, "Highscores", backgroundColor, backgroundColor, 3);
	lcdScreen->drawInteger(50, 50, highscores[3][1], DEC, backgroundColor, backgroundColor, 3);
	lcdScreen->drawInteger(50, 80, highscores[2][1], DEC, backgroundColor, backgroundColor, 3);
	lcdScreen->drawInteger(50, 110, highscores[1][1], DEC, backgroundColor, backgroundColor, 3);
	lcdScreen->drawInteger(50, 140, highscores[0][1], DEC, backgroundColor, backgroundColor, 3);
	lcdScreen->fillRect(10, 200, 30, 30, backgroundColor);
	lcdScreen->fillRect(40, 70, 130, 160, backgroundColor);
	drawMenuText();
}

void ScreensClass::drawHighscoresText() {	// Sets the highscores screen's "Highscores" text by drawing it to the screen and draws the highscores that have been written to EEPROM
	lcdScreen->drawText(50, 20, "Highscores", textColor, backgroundColor, 3);
	lcdScreen->drawInteger(50, 50, highscores[3][1], DEC, textColor, backgroundColor, 3);
	lcdScreen->drawInteger(50, 80, highscores[2][1], DEC, textColor, backgroundColor, 3);
	lcdScreen->drawInteger(50, 110, highscores[1][1], DEC, textColor, backgroundColor, 3);
	lcdScreen->drawInteger(50, 140, highscores[0][1], DEC, textColor, backgroundColor, 3);
}

void ScreensClass::drawHighscoresBackButton() {	// Sets the highscores screen's back button by filling a rectangle on the screen
	lcdScreen->fillRect(10, 200, 30, 30, textColor);
}

void ScreensClass::drawHighscoresList() {	// Draws the highscores
	uint8_t i, j, tempScore, tempPlayer;
	highscores[0][0] = 0;
	highscores[0][1] = EEPROM.read(0);	// Reads the highscore values from EEPROM
	highscores[1][0] = 1;
	highscores[1][1] = EEPROM.read(1);
	highscores[2][0] = 2;
	highscores[2][1] = EEPROM.read(2);
	highscores[3][0] = 3;
	highscores[3][1] = EEPROM.read(3);

	for (i = 0; i < 4; i++) {	// Sorts the highscore list from high to low
		for (j = i + 1; j < 4; j++) {
			if (highscores[i][1] > highscores[j][1]) {
				tempScore = highscores[i][1];
				highscores[i][1] = highscores[j][1];
				highscores[j][1] = tempScore;

				tempPlayer = highscores[i][0];
				highscores[i][0] = highscores[j][0];
				highscores[j][0] = tempPlayer;
			}
		}
	}
}

void ScreensClass::drawWinScreen(uint8_t player) {	// Draws the win screen
	uint8_t i, j, tempScore, tempPlayer;
	highscores[0][0] = 0;
	highscores[0][1] = EEPROM.read(0);	// Reads highscore values
	highscores[1][0] = 1;
	highscores[1][1] = EEPROM.read(1);
	highscores[2][0] = 2;
	highscores[2][1] = EEPROM.read(2);
	highscores[3][0] = 3;
	highscores[3][1] = EEPROM.read(3);
	switch (player) {
	case(1):
		lcdScreen->drawText(53, 100, "Player 1 Wins!", textColor, backgroundColor, 2);
		lcdScreen->drawText(65, 140, "Press screen to continue", textColor, backgroundColor, 1);
		break;
	case(2):
		lcdScreen->drawText(53, 100, "Player 2 Wins!", textColor, backgroundColor, 2);
		lcdScreen->drawText(65, 140, "Press screen to continue", textColor, backgroundColor, 1);
		break;
	}
	if (player == 1) {
		lcdScreen->drawText(70, 160, "Winning score: ", textColor, backgroundColor, 1);	// Draw's player 1's score on the screen
		lcdScreen->drawInteger(200, 160, tank1->getScore(), DEC, textColor, backgroundColor, 1);
		for (i = 0; i < 4; i++) {	// Sorts highscores list
			for (j = i + 1; j < 4; j++) {
				if (highscores[i][1] > highscores[j][1]) {
					tempScore = highscores[i][1];
					highscores[i][1] = highscores[j][1];
					highscores[j][1] = tempScore;

					tempPlayer = highscores[i][0];
					highscores[i][0] = highscores[j][0];
					highscores[j][0] = tempPlayer;
				}
			}
		}
		if (tank1->getScore() > highscores[0][1]) {	// If the current score is higher than the lowest highscore it will overwrite it
			highscores[0][1] = tank1->getScore();
			EEPROM.write(0, highscores[0][1]);
			EEPROM.write(1, highscores[1][1]);
			EEPROM.write(2, highscores[2][1]);
			EEPROM.write(3, highscores[3][1]);
		}
	}
	else {
		lcdScreen->drawText(70, 160, "Winning score: ", textColor, backgroundColor, 1);	// Draw's player 2's score on the screen
		lcdScreen->drawInteger(200, 160, tank2->getScore(), DEC, textColor, backgroundColor, 1);
		for (i = 0; i < 4; i++) {	// Sorts highscores list
			for (j = i + 1; j < 4; j++) {
				if (highscores[i][1] > highscores[j][1]) {
					tempScore = highscores[i][1];
					highscores[i][1] = highscores[j][1];
					highscores[j][1] = tempScore;

					tempPlayer = highscores[i][0];
					highscores[i][0] = highscores[j][0];
					highscores[j][0] = tempPlayer;
				}
			}
		}
		if (tank2->getScore() > highscores[0][1]) {	// If the current score is higher than the lowest highscore it will overwrite it
			highscores[0][1] = tank2->getScore();
			EEPROM.write(0, highscores[0][1]);
			EEPROM.write(1, highscores[1][1]);
			EEPROM.write(2, highscores[2][1]);
			EEPROM.write(3, highscores[3][1]);
		}
	}
}

void ScreensClass::drawHealthText(TankClass * tank1, TankClass * tank2) { // Draws the tanks' health to the screen
	String health = (String) tank1->getHealth();
	lcdScreen->drawText(10, 10, health, RGB(255, 0, 0), backgroundColor, 2);
	health = (String) tank2->getHealth();
	lcdScreen->drawText(300, 220, health, RGB(255, 0, 0), backgroundColor, 2);

}

uint8_t ScreensClass::gameBackPressed() {	// Checks if the game's back button is pressed
	if ((lcdScreen->touchRead() == 1 && lcdScreen->touchX() >= 149 && lcdScreen->touchX() <= 170 && lcdScreen->touchY() >= 10 && lcdScreen->touchY() <= 31)) {
		return 1;
	}
	else {
		return 0;
	}
}

uint8_t ScreensClass::highscoresBackPressed() {	// Checks if the highscores' back button is pressed
	if (lcdScreen->touchRead() == 1 && lcdScreen->touchX() >= 10 && lcdScreen->touchX() <= 40 && lcdScreen->touchY() >= 200 && lcdScreen->touchY() <= 230) {
		return 1;
	}
	else {
		return 0;
	}
}


uint8_t ScreensClass::startPressed() {	// Checks if "Start" is pressed
	if (lcdScreen->touchRead() == 1 && lcdScreen->touchX() >= 105 && lcdScreen->touchX() <= 240 && lcdScreen->touchY() >= 75 && lcdScreen->touchY() <= 100) {
		startPressedInt = 1;

		return 1;
	}
	else {
		return 0;
	}
}

uint8_t ScreensClass::highscoresPressed() {	// Checks if "Highscores" is pressed
	if (lcdScreen->touchRead() == 1 && lcdScreen->touchX() >= 55 && lcdScreen->touchX() <= 300 && lcdScreen->touchY() >= 125 && lcdScreen->touchY() <= 160) {
		highscoresPressedInt = 1;
		return 1;
	}
	else {
		return 0;
	}
}

uint8_t ScreensClass::screenPressed() { // If the screen is pressed
	if (lcdScreen->touchRead() == 1) {
		return 1;
	}
	else {
		return 0;
	}
}

void ScreensClass::setStartPressedInt(uint8_t startPressed) {	// Sets the startPressedInt
	startPressedInt = startPressed;
}

void ScreensClass::setHighscoresPressedInt(uint8_t highscoresPressed) {	// Sets the highscoresPressedInt
	highscoresPressedInt = highscoresPressed;
}

uint8_t ScreensClass::getStartPressedInt() {	// Returns the startPressedInt
	return startPressedInt;
}

uint8_t ScreensClass::getHighscoresPressedInt() {	// Returns the highscoresPressedInt
	return highscoresPressedInt;
}

void ScreensClass::setStartInit(uint8_t startInit) {	// Set the start game initialize variable
	this->startInit = startInit;
}

uint8_t ScreensClass::getStartInit() {	// Gets the start game initialize variable
	return startInit;
}

void ScreensClass::setFirstRun(uint8_t firstRun) {	// Sets the firstRun variable
	this->firstRun = firstRun;
}

ScreensClass Screens;