#include "MI0283QT9.h"
#include "ArduinoNunchuk.h"
#include "Grid.h"

#ifndef _TANK_h
#define _TANK_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class TankClass {
protected:
	MI0283QT9 * lcd;				// Pointer for LCD
	GridClass * Grid;				// Pointer for Grid Class
	ArduinoNunchuk * Nunchuk;		// Arduino nunchuck
	TankClass * opponentTank;		// Pointer to opponent tank
	uint8_t side;					// 1 = left, 2 = right
	uint8_t analogX;				// Nunchuck X value
	uint8_t analogY;				// Nunchuck Y value
	uint8_t zButton;
	uint8_t zButtonPressed;
	uint8_t zButtonReleased;
	uint8_t bulletFired;
	uint16_t xMaxField;				// Maximum X value of field
	uint16_t xMinField;				// Minimal X value of field
	uint8_t yMaxField;				// Maximum Y value of field
	uint8_t yMinField;				// Maximum Y value of field
	uint8_t tankSpeed;				// Speed of tank
	uint16_t tankPosX;				// Tank position X
	uint8_t tankPosY;				// Tank position Y
	uint16_t tankPosOldX;			// Old tank position X
	uint8_t tankPosOldY;			// Old tank position Y
	uint16_t bulletPosX;			// Bullet x position
	uint8_t bulletPosY;				// Bullet y position

	uint8_t bulletSize;
	uint8_t bulletSpeed;
	uint8_t health;
	uint8_t player;					// 1 = self, 2 = opponent
	uint16_t score;

	uint16_t bulletHitboxX1;		// hitbox bullet
	uint8_t bulletHitboxY1;
	uint16_t bulletHitboxX2;
	uint8_t bulletHitboxY2;

	uint16_t tankHitboxX1;			// hitbox tank
	uint8_t tankHitboxY1;
	uint16_t tankHitboxX2;
	uint8_t tankHitboxY2;

	void drawTank(uint16_t x, uint8_t y);
	void removeTank(uint16_t x, uint8_t y);
	void drawBullet(int x, int y);
	void removeBullet(int x, int y);
	void setTankHitbox();
	void setBulletHitbox();
public:
	void init(MI0283QT9 * lcd, uint8_t side, GridClass * grid, ArduinoNunchuk * nunchuk, TankClass * opponentTank);
	void resetTank();
	void resetBullet();
	void moveTank();
	uint8_t isHit();
	uint8_t pauseIsHit();
	void lowerHealth();
	void shootBullet();

	void setScore(uint16_t s);
	uint16_t getScore();
	void setHealth(uint8_t health);
	uint8_t getHealth();
	void setTankPosX(uint16_t x);
	uint16_t getTankPosX();
	void setTankPosY(uint8_t y);
	uint8_t getTankPosY();
	uint8_t getSide();
	void setPlayer(uint8_t player);
	uint8_t getPlayer();
	void setBulletFired(uint8_t bullet);
	uint8_t getBulletFired();
};

#endif