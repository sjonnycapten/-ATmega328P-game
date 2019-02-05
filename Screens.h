#include "MI0283QT9.h"
#include <EEPROM.h>
#include "Tank.h"
#include "Grid.h"


#ifndef _SCREENS_h
#define _SCREENS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


class ScreensClass {
protected:
	MI0283QT9 * lcdScreen;			//Pointer for LCD
	GridClass *grid;
	int highscores[4][2];
	char orderedHighscores[8];
	uint8_t highscoresPressedInt;
	uint8_t startPressedInt;
	uint8_t startInit;
	uint8_t playerWin;
	uint8_t firstRun;
	uint16_t scoreCount1;
	uint16_t scoreCount2;
	TankClass * tank1;
	TankClass * tank2;
	uint8_t player;
public:
	void setMenu(uint8_t connected);
	void init(MI0283QT9 * lcdScreen, GridClass *grid, TankClass * tank1, TankClass * tank2);
	void startGame();
	void refreshGame();
	void drawMenuBackground();
	void drawMenuText();
	void setGameBackButton();
	void drawHighscoresScreen();
	void removeHighscoresScreen();
	void drawHighscoresText();
	void drawHighscoresBackButton();
	void drawHighscoresList();
	void drawWinScreen(uint8_t player);
	void drawHealthText(TankClass * tank1, TankClass * tank2);
	uint8_t gameBackPressed();
	uint8_t highscoresBackPressed();
	uint8_t startPressed();
	uint8_t highscoresPressed();
	uint8_t screenPressed();
	void setStartPressedInt(uint8_t startPressed);
	void setHighscoresPressedInt(uint8_t highscoresPressed);
	uint8_t getStartPressedInt();
	uint8_t getHighscoresPressedInt();
	void setStartInit(uint8_t startInit);
	uint8_t getStartInit();
	void setFirstRun(uint8_t firstRun);
	void clearHighscores();
};

extern ScreensClass Screens;

#endif