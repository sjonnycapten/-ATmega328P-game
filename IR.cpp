#include "IR.h"

#define DATA_LENGTH 20		//Length of data in buffer
#define PULSE 7				//Time to send and receive 1 bit
#define XTANKLENGTH 9		//Amount of bits for x value of tank
#define YTANKLENGTH 8		//Amount of bits for y value of tank
#define BULLET 1			//Amount of bits for bullet fired value
#define HEALTH 2			//Amount of bits for tank hit value


void IRClass::init(char *mainBufferSend, char *inverseBufferSend, char *mainBufferReceive, char *inverseBufferReceive, TankClass *tankLeft, TankClass *tankRight, ScreensClass *screen, volatile uint16_t *counterSend, volatile uint16_t *counterReceive, volatile uint16_t *connectionTime, volatile uint8_t *inputChange, uint8_t player) {
	this->mainBufferSend = new char[DATA_LENGTH];
	this->mainBufferSend = mainBufferSend;
	this->inverseBufferSend = new char[DATA_LENGTH];
	this->inverseBufferSend = inverseBufferSend;
	this->mainBufferReceive = new char[DATA_LENGTH];
	this->mainBufferReceive = mainBufferReceive;
	this->inverseBufferReceive = new char[DATA_LENGTH];
	this->inverseBufferReceive = inverseBufferReceive;
	this->ownTank = new TankClass;
	this->opponentTank = new TankClass;
	this->screen = new ScreensClass;
	this->screen = screen;
	this->counterSend = new volatile uint16_t;
	this->counterSend = counterSend;
	this->counterReceive = new volatile uint16_t;
	this->counterReceive = counterReceive;
	this->connectionTime = new volatile uint16_t;
	this->connectionTime = connectionTime;
	this->inputChange = new volatile uint8_t;
	this->inputChange = inputChange;
	this->player = player;
	this->receive = 1;
	this->refreshGame = 0;

	switch (player) {
	case(1):
		ownTank = tankLeft;
		opponentTank = tankRight;
		break;
	case(2):
		ownTank = tankRight;
		opponentTank = tankLeft;
		break;
	}
}

void IRClass::run() {
	if (receive) {						// When the receive variable is true, data will be encoded and sent
		receive = 0;
		encodeData();
		sendStartData();
		sendData();
		sendEndData();
		*inputChange = 0;
	}
	if (*inputChange >= 2) {			// When there are 2 external interrupts, data will be received and stored
		*connectionTime = 0;
		EIMSK &= ~(1 << INT0);			// External interrupt disable
		receiveData();
		receive = 1;
		if (checkData()) {				// When data is received correctly, values will be decoded and updated and screen will be updated
			decodeXTank();
			decodeYTank();
			decodeBullet();
			decodeHealth();
			refreshGame = 1;
			*connectionTime = 0;
		}
		else {
			refreshGame = 0;
		}
		EIMSK |= (1 << INT0);			// External interrupt enable
		*inputChange = 0;
	}
	if (*connectionTime >= 5000) {		// When there is too long no connection, the screen won't update
		refreshGame = 0;
		*connectionTime = 0;
	}
}

void IRClass::setRefreshGame(uint8_t value) {
	this->refreshGame = value;
}

uint8_t IRClass::getRefreshGame() {
	return refreshGame;
}

void IRClass::encodeData() {
	this->xTank = ownTank->getTankPosX();
	this->yTank = ownTank->getTankPosY();
	this->bullet = ownTank->getBulletFired();
	this->health = ownTank->getHealth();
	int j;
	uint8_t k;
	uint8_t count = 0;
	for (j = XTANKLENGTH - 1; j >= 0; j--) {														// Start at the XTANKLENGTH bits form to right and fill those bits from right to left, until the start of the array is reached.
		if (this->xTank & (1 << count)) {															// If bitwise of AND of the Xposition and the bitmask made by count is 1.(bit is one) 
			mainBufferSend[j] = '1';																//	Fill buffer with one
		}
		else {
			mainBufferSend[j] = '0';																// Fill buffer with zero
		}
		count++;																					// Increase count so the mask will be updated.
	}
	count = 0;																						// Reset count so the mask can be reused 
	for (j = XTANKLENGTH + YTANKLENGTH - 1; j >= XTANKLENGTH; j--) {					            // Start at the XTANKLENGTH + YTANKLENGTH bits form to right and fill those bits from right to left, until bits set by XTANKLENGTH are reached.
		if (this->yTank & (1 << count)) {															// If bitwise of AND of the YtankPostion and the bitmask made by count is 1.(bit is one) 
			mainBufferSend[j] = '1';																//	Fill buffer with one
		}
		else {
			mainBufferSend[j] = '0';																// Fill buffer with zero
		}
		count++;																					// Increase count so the mask will be updated.
	}
								 
	count = 0;																						  // Reset count so the mask can be reused																						
	for (j = XTANKLENGTH + YTANKLENGTH + BULLET - 1; j >= XTANKLENGTH + YTANKLENGTH ; j--) {		  // Start at the XTANKLENGTH + YTANKLENGTH + BULLET bits form to right and fill those bits from right to left, until bits set by YTANKLENGTH are reached.      
		if (this->bullet & (1 << count)) {															  // If bitwise of AND of the bullet and the bitmask made by count is 1.(bit is one) 
			mainBufferSend[j] = '1';																  // Fill buffer with one
		}
		else {
			mainBufferSend[j] = '0';																  // Fill buffer with zero
		}
		count++;																					  // Increase count so the mask will be updated.
	}
	count = 0;
	for (j = XTANKLENGTH + YTANKLENGTH + BULLET + HEALTH - 1; j >= XTANKLENGTH + YTANKLENGTH +  BULLET; j--) {        // Start at the XTANKLENGTH + YTANKLENGTH + BULLET + HEALTH bits form to right and fill those bits from right to left, until bits set by BULLET are reached.      
		if (this->health & (1 << count)) {																			  // If bitwise of AND of the health and the bitmask made by count is 1.(bit is one) 
			mainBufferSend[j] = '1';																				  // Fill buffer with one
		}
		else {
			mainBufferSend[j] = '0';																				  // Fill buffer with zero
		}
		count++;																									  // Increase count so the mask will be updated.
	}

	for (k = 0; k < DATA_LENGTH; k++) {																						//filling inverse buffer
		if (mainBufferSend[k] == '1') {
			inverseBufferSend[k] = '0';
		}
		else {
			inverseBufferSend[k] = '1';
		}
	}
}

void IRClass::sendStartData() {
	TCCR2A &= ~(1 << COM2B1);
	*counterSend = 0;
	while (*counterSend <= (PULSE * 4)) {		// 4 0's will be sent
	}
	TCCR2A |= (1 << COM2B1);
	*counterSend = 0;
	while (*counterSend <= (PULSE * 2)) {		// 2 1's will be sent
	}
	TCCR2A &= ~(1 << COM2B1);
	*counterSend = 0;
	while (*counterSend <= PULSE) {				// 1 0 will be sent
	}
	TCCR2A |= (1 << COM2B1);
	*counterSend = 0;
	while (*counterSend <= PULSE) {				// 1 1 will be sent
	}
}

void IRClass::sendData() {
	uint8_t j;
	uint8_t k;
	for (j = 0; j < DATA_LENGTH; j++) {
		if (mainBufferSend[j] == '0') {		// Turn IR sensor on or off
			TCCR2A |= (1 << COM2B1);
		}
		else {
			TCCR2A &= ~(1 << COM2B1);
		}
		*counterSend = 0;					// Reset countervalue
		while (*counterSend <= PULSE) {		// Wait until time is over for 1 bit
		}
	}

	for (k = 0; k < DATA_LENGTH; k++) {		// Turn IR sensor on or off
		if (inverseBufferSend[k] == '0') {
			TCCR2A |= (1 << COM2B1);
		}
		else {
			TCCR2A &= ~(1 << COM2B1);
		}
		*counterSend = 0;					// Reset countervalue
		while (*counterSend <= PULSE) {		// Wait until time is over for 1 bit		
		}
	}
}

void IRClass::sendEndData() {
	TCCR2A &= ~(1 << COM2B1);
	*counterSend = 0;
	while (*counterSend <= (PULSE)) {		// 1 0 will be sent
	}
}

void IRClass::receiveData() {
	uint8_t j;
	uint8_t k;
	*counterReceive = 0;
	while (*counterReceive <= (PULSE + PULSE)) {					// Start bits will be skipped
	}
	for (j = 0; j < DATA_LENGTH; j++) {								// Read IR receiver for length of 1 bit
		zero = 0;													// Reset amount of zero's
		one = 0;													// Reset amount of one's
		*counterReceive = 0;										// Reset countervalue
		while (*counterReceive <= PULSE) {
			if (PIND & (1 << PIND2)) {								// When input is high, higher one; when input is low, higher zero
				zero++;
			}
			else {
				one++;
			}
		}
		if (zero > one) {											// When there are more zero's then one's, put '0' in buffer; else put '1' in buffer
			mainBufferReceive[j] = '1';
		}
		else {
			mainBufferReceive[j] = '0';
		}
	}
	for (k = DATA_LENGTH; k < DATA_LENGTH + DATA_LENGTH; k++) {		// Fill inverseBuffer with bits
		zero = 0;
		one = 0;
		*counterReceive = 0;
		while (*counterReceive <= PULSE) {
			if (PIND & (1 << PIND2)) {
				zero++;
			}
			else {
				one++;
			}
		}
		if (zero > one) {
			inverseBufferReceive[k - DATA_LENGTH] = '1';
		}
		else {
			inverseBufferReceive[k - DATA_LENGTH] = '0';
		}
	}
}

void IRClass::decodeXTank() {									// Decode the buffer part xTank.
	int x;
	uint16_t output = 0;										// Used to hold the final value.
	uint8_t count = 0;											// Holds postion of curssor in array.
	for (x = XTANKLENGTH - 1; x >= 0; x--) {                    // For each bit in the XTANKLENGHT part of the buffer.
		if (mainBufferReceive[x] == '1') {						// If bit it 1.
			output += (uint16_t)(pow(2.00, count) + 0.5);		// Add 2 to the power of count.
		}
		count++;
	}
	opponentTank->setTankPosX(output);							// Set the postion of the opponent Tank
}

void IRClass::decodeYTank() {											// Decode the buffer part yTank.	
	int x;
	uint8_t output = 0;													// Used to hold the final value.
	uint8_t count = 0;													// Holds postion of curssor in array.                   	
	for (x = XTANKLENGTH + YTANKLENGTH - 1; x >= XTANKLENGTH; x--) {	// For each bit in the YTANKLENGHT part of the buffer.              
		if (mainBufferReceive[x] == '1') {								// If bit it 1.	
			output += (uint8_t)(pow(2.00, count) + 0.5);				// Add two to the power of count to output.	
		}
		count++;
	}																	
	opponentTank->setTankPosY(output);									// Set the postion of the opponent Tank
}



void IRClass::decodeBullet() {																	// Decode the buffer part BULLET.	
	int x;
	uint8_t output = 0;																			// Used to hold the final value.
	uint8_t count = 0;																			// Holds postion of curssor in array.                  
	for (x = XTANKLENGTH + YTANKLENGTH + BULLET - 1; x >= XTANKLENGTH + YTANKLENGTH; x--) {     // For each bit in the BULLET part of the buffer.                               
		if (mainBufferReceive[x] == '1') {														// If bit it 1.	
			output += (uint8_t)(pow(2.00, count) + 0.5);										// Add two to the power of count to output.	
		}
		count++;
	}
	opponentTank->setBulletFired(output);													// Set the postion of the opponent Tank
}

void IRClass::decodeHealth() {																					// Decode the buffer part HEALTH.	                         
	int x;
	uint8_t output = 0;																							// Used to hold the final value.
	uint8_t count = 0;																							// Holds postion of curssor in array.                  
	for (x = XTANKLENGTH + YTANKLENGTH + BULLET + HEALTH - 1; x >= XTANKLENGTH + YTANKLENGTH + BULLET; x--) {   // For each bit in the HEALTH part of the buffer.                             
		if (mainBufferReceive[x] == '1') {																		// If bit it 1.	
			output += (uint8_t)(pow(2.00, count) + 0.5);														// Add two to the power of count to output.	
		}
		count++;
	}
	opponentTank->setHealth(output);																	// Set the postion of the opponent Tank
}

uint8_t IRClass::checkData() {
	uint8_t j;
	uint8_t correct;
	for (j = 0; j < DATA_LENGTH; j++) {								// Every item in buffers is checked on corruption
		if (mainBufferReceive[j] != inverseBufferReceive[j]) {		// If item is not the same as inverted item, correct will be highered
			correct++;
		}
	}
	if (correct == DATA_LENGTH) {									// If correct is equal to amount of bits - startbits, true will be returned
		return 1;
	}
	else {
		return 0;
	}
}

IRClass IR;