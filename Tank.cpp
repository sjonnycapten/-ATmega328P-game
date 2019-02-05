#include "Tank.h"

#define BGC RGB(212,212,212)
#define TANKTOP RGB(255,0,0)
#define TANKBOTTOM RGB(0,255,72)
#define BULLETCOLOR  RGB(96,125,139)

uint8_t zButtonReleased = 0;				// Nunchuk Z button released

void TankClass::init(MI0283QT9 * lcd, uint8_t side, GridClass * Grid, ArduinoNunchuk * nunchuk, TankClass * opponentTank) {	//Tank initialization
	this->Nunchuk = nunchuk;				//Nunchuk initialization
	this->Grid = Grid;
	this->lcd = lcd;
	this->side = side;
	this->opponentTank = opponentTank;
	yMaxField = 235;						// Set y max field
	yMinField = 13;							// Set y min field
	switch (side) {
	case (1):								// Default x field settings for player left
		xMaxField = 148;
		xMinField = 10;
		break;
	case(2):								// Default y field settings for player right
		xMaxField = 305;
		xMinField = 170;
		break;
	}
}

void TankClass::resetTank() {
	score = 0;								// Reset score
	bulletSize = 5.5;						// Set bulletspeed to 5.5 pixel per refresh
	bulletSpeed = 12;						// Set bulletspeed
	tankSpeed = 4;							// Set tank movement speed to 4 pixels per move
	health = 3;								// Sets tank health to 3
	removeTank(tankPosX, tankPosY);			// Remove old tank
	switch (side) {
	case (1):
		tankPosX = xMinField;				// Sets tank starting positions
		tankPosY = yMinField;				//
		bulletPosX = 350;					// Resets bullet positions
		bulletPosY = 240;					//
		break;
	case (2):
		tankPosX = xMaxField;				// Sets tank starting positions
		tankPosY = yMaxField;				//
		bulletPosX = 0;						// Resets bullet positions
		bulletPosY = 0;						//
		break;
	}
	drawTank(tankPosX, tankPosY);			// Draw new tank
}

void TankClass::drawTank(uint16_t x, uint8_t y) {
	lcd->drawLine(x - 7, y, x + 8, y, TANKBOTTOM);
	lcd->drawLine(x - 8, y - 1, x + 9, y - 1, TANKBOTTOM);
	lcd->drawLine(x - 8, y - 2, x + 9, y - 2, TANKBOTTOM);
	lcd->drawLine(x - 7, y - 3, x + 8, y - 3, TANKBOTTOM);
	lcd->drawLine(x - 6, y - 4, x + 7, y - 4, TANKTOP);
	lcd->drawLine(x - 6, y - 5, x + 7, y - 5, TANKTOP);
	lcd->drawLine(x - 5, y - 6, x + 6, y - 6, TANKTOP);
	lcd->drawLine(x - 4, y - 7, x + 5, y - 7, TANKTOP);
	lcd->drawLine(x - 3, y - 8, x + 4, y - 8, TANKTOP);
	lcd->drawLine(x - 2, y - 9, x + 3, y - 9, TANKTOP);
	lcd->drawLine(x - 1, y - 10, x + 2, y - 10, TANKTOP);
}

void TankClass::removeTank(uint16_t x, uint8_t y) {
	lcd->drawLine(x - 7, y, x + 8, y, BGC);
	lcd->drawLine(x - 8, y - 1, x + 9, y - 1, BGC);
	lcd->drawLine(x - 8, y - 2, x + 9, y - 2, BGC);
	lcd->drawLine(x - 7, y - 3, x + 8, y - 3, BGC);
	lcd->drawLine(x - 6, y - 4, x + 7, y - 4, BGC);
	lcd->drawLine(x - 6, y - 5, x + 7, y - 5, BGC);
	lcd->drawLine(x - 5, y - 6, x + 6, y - 6, BGC);
	lcd->drawLine(x - 4, y - 7, x + 5, y - 7, BGC);
	lcd->drawLine(x - 3, y - 8, x + 4, y - 8, BGC);
	lcd->drawLine(x - 2, y - 9, x + 3, y - 9, BGC);
	lcd->drawLine(x - 1, y - 10, x + 2, y - 10, BGC);
}

void TankClass::moveTank() {														// Moving the tank
	if (player == 1) {																// Read nunchuck only by own player
		analogX = Nunchuk->getAnalogX();											// Read X value of nunchuck
		analogY = Nunchuk->getAnalogY();											// Red Y value of nunchuck
		if (analogX < 100 || analogX > 150 || analogY < 105 || analogY > 155) {		// If nunchuck is moved out of deadzone
			if (analogX < 100 && tankPosX >= xMinField + tankSpeed) {
				tankPosX -= tankSpeed;
			}
			if (analogX > 150 && tankPosX <= xMaxField - tankSpeed) {
				tankPosX += tankSpeed;
			}
			if (analogY < 105 && tankPosY <= yMaxField - tankSpeed) {
				tankPosY += tankSpeed;
			}
			if (analogY > 155 && tankPosY >= yMinField + tankSpeed) {
				tankPosY -= tankSpeed;
			}
		}
	}
	if (!(tankPosOldX == tankPosX && tankPosOldY == tankPosY)) {					// Draw only by new position
		removeTank(tankPosOldX, tankPosOldY);										// Remove old tank
		drawTank(tankPosX, tankPosY);												// Draw new tank
		tankPosOldX = tankPosX;														// New positions to old positions
		tankPosOldY = tankPosY;														//
		setTankHitbox();															// Set new hitbox
	}
	shootBullet();
}

void TankClass::setTankHitbox() {		// Sets the tank's hitbox
	tankHitboxX1 = tankPosX - 8;
	tankHitboxY1 = tankPosY - 10;
	tankHitboxX2 = tankPosX + 9;
	tankHitboxY2 = tankPosY;
}

void TankClass::setBulletHitbox() {		// Sets the bullet's hitbox
	bulletHitboxX1 = bulletPosX - 5;
	bulletHitboxY1 = bulletPosY - 5;
	bulletHitboxX2 = bulletPosX + 5;
	bulletHitboxY2 = bulletPosY + 5;
}

void TankClass::resetBullet() {			// Resets the bullet's positions and removes it from the screen
	removeBullet(bulletPosX, bulletPosY);
	switch (side) {
	case (1):
		bulletPosX = 340;
		bulletPosY = 250;
		break;
	case (2):
		bulletPosX = 0;
		bulletPosY = 0;
		break;
	}
}

uint8_t TankClass::isHit() {
	TankClass * tank = opponentTank;
	if (((bulletHitboxX1 >= tank->tankHitboxX1 && bulletHitboxX1 <= tank->tankHitboxX2) && (bulletHitboxY1 >= tank->tankHitboxY1 && bulletHitboxY1 <= tank->tankHitboxY2)) ||
		((bulletHitboxX2 >= tank->tankHitboxX1 && bulletHitboxX2 <= tank->tankHitboxX2) && (bulletHitboxY1 >= tank->tankHitboxY1 && bulletHitboxY1 <= tank->tankHitboxY2)) ||
		((bulletHitboxX1 >= tank->tankHitboxX1 && bulletHitboxX1 <= tank->tankHitboxX2) && (bulletHitboxY2 >= tank->tankHitboxY1 && bulletHitboxY2 <= tank->tankHitboxY2)) ||
		((bulletHitboxX2 >= tank->tankHitboxX1 && bulletHitboxX2 <= tank->tankHitboxX2) && (bulletHitboxY2 >= tank->tankHitboxY1 && bulletHitboxY2 <= tank->tankHitboxY2))) {	// If tank is hit
		return 1;
	}
	else {
		return 0;
	}
}

uint8_t TankClass::pauseIsHit() {// Coords pause circle: x1: 148 y1: 4 x2: 172 y2: 28
	if (((bulletHitboxX1 >= 143 && bulletHitboxX1 <= 177) && (bulletHitboxY1 >= 0 && bulletHitboxY1 <= 33)) ||
		((bulletHitboxX2 >= 143 && bulletHitboxX2 <= 177) && (bulletHitboxY1 >= 0 && bulletHitboxY1 <= 33)) ||
		((bulletHitboxX1 >= 143 && bulletHitboxX1 <= 177) && (bulletHitboxY2 >= 0 && bulletHitboxY2 <= 33)) ||
		((bulletHitboxX2 >= 143 && bulletHitboxX2 <= 177) && (bulletHitboxY2 >= 0 && bulletHitboxY2 <= 33))) { // If bullet hits pause 
		return 1;
	}
	else if (((tankHitboxX1 >= 143 && tankHitboxX1 <= 177) && (tankHitboxY1 >= 0 && tankHitboxY1 <= 33)) ||
		((tankHitboxX2 >= 143 && tankHitboxX2 <= 177) && (tankHitboxY1 >= 0 && tankHitboxY1 <= 33)) ||
		((tankHitboxX1 >= 143 && tankHitboxX1 <= 177) && (tankHitboxY2 >= 0 && tankHitboxY2 <= 33)) ||
		((tankHitboxX2 >= 143 && tankHitboxX2 <= 177) && (tankHitboxY2 >= 0 && tankHitboxY2 <= 33))) {	// If tank hits pause
		return 1;
	}
	else {
		return 0;
	}
}



void TankClass::lowerHealth() {	// Lower the tank's health by 1
	if (health > 0) {
		health = health - 1;
		if (side == 1) {
			setScore(getScore() + 10 + ((opponentTank->getTankPosX() - getTankPosX()) / 10)); // Calculate score
		}
		else {
			setScore(getScore() + 10 + ((getTankPosX() - opponentTank->getTankPosX()) / 10));
		}
	}
	else {
		health = 3;
	}
}

void TankClass::shootBullet() {				// Shooting the bullet
	if (player == 1) {						// Read nunchuck only by own player
		zButton = Nunchuk->getZButton();	// Read Z value of nunchuck 1
	}

	switch (side) {							// Switch for determining which tank is which so it applies properties to the bullet
	case (1):								// Bullet for tank left
		if (zButton && !bulletFired) {
			zButtonReleased = 0;
			bulletPosX = tankPosX + 15;	// Bullet spawns to the right of the tank
			bulletPosY = tankPosY - 5;
			zButtonPressed = 1;
		}

		if (!zButton && zButtonPressed) {	// If Z button isn't pressed, but has been pressed before
			zButtonReleased = 1;
			bulletFired = 1;
			zButtonPressed = 0;
		}

		if (bulletPosX > 319) {	// If bullet is at end of screen
			bulletFired = 0;
			resetBullet();
		}

		if (bulletPosX < 319 && zButtonReleased) {	// If bullet is on screen and the Z button has been pressed and released
			removeBullet(bulletPosX, bulletPosY);
			bulletPosX += bulletSpeed;
			drawBullet(bulletPosX, bulletPosY);
			setBulletHitbox();
		}
		break;
	case (2):									// Bullet for tank right
		if (zButton && !bulletFired) {
			zButtonReleased = 0;
			bulletPosX = tankPosX - 15;	// Bullet spawns to the left of the tank
			bulletPosY = tankPosY - 5;
			zButtonPressed = 1;
		}

		if (!zButton && zButtonPressed) {	// If Z button isn't pressed, but has been pressed before
			zButtonReleased = 1;
			bulletFired = 1;
			zButtonPressed = 0;
		}

		if (bulletPosX > (bulletSize * 2) && zButtonReleased) {	// If bullet is on screen and the Z button has been pressed and released
			removeBullet(bulletPosX, bulletPosY);
			bulletPosX -= bulletSpeed;
			drawBullet(bulletPosX, bulletPosY);
			setBulletHitbox();
		}
		else if (bulletPosX <= (bulletSize * 2)) {	// If bullet is at end of screen
			resetBullet();
			bulletFired = 0;
		}
		break;
	}
}

void TankClass::drawBullet(int x, int y) {		// Draws the bullet
	lcd->fillCircle(x, y, bulletSize, BULLETCOLOR);
}

void TankClass::removeBullet(int x, int y) {	// Removes the bullet
	lcd->fillCircle(x, y, bulletSize, BGC);
}

uint16_t TankClass::getScore() {
	return score;
}

void TankClass::setScore(uint16_t s) {
	score = s;
}

uint8_t TankClass::getBulletFired() {
	return bulletFired;
}

void TankClass::setBulletFired(uint8_t bullet) {
	zButton = !bullet;
}

uint8_t TankClass::getSide() {
	return side;
}

void TankClass::setPlayer(uint8_t player) {
	this->player = player;					// Set player (1 = self, 2 = opponent)
}

uint8_t TankClass::getPlayer() {
	return player;
}

void TankClass::setTankPosX(uint16_t x) {
	tankPosX = x;
}

uint16_t TankClass::getTankPosX() {
	return tankPosX;
}

void TankClass::setTankPosY(uint8_t y) {
	tankPosY = y;
}

uint8_t TankClass::getTankPosY() {
	return tankPosY;
}

void TankClass::setHealth(uint8_t health) { // Sets the health for opponent
	this->health = health;
}

uint8_t TankClass::getHealth() { // Gets the tank's health
	return health;
}



