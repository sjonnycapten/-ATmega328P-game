#include "Tank.h"
#include "Screens.h"
#ifndef _IR_h
#define _IR_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class IRClass {
protected:
	char *mainBufferSend;					//Pointer to main data to send
	char *inverseBufferSend;				//Pointer to inverse data to send
	char *mainBufferReceive;				//Pointer to main data received
	char *inverseBufferReceive;				//Pointer to inverse data received

	volatile uint16_t *counterSend;			//Countervalue sending
	volatile uint16_t *counterReceive;		//Countervalue receiving
	volatile uint8_t *inputChange;			//Value of input changes
	volatile uint16_t *connectionTime;		//Value of time for offline connection

	uint16_t one;							//Amount of one's in received data
	uint16_t zero;							//Amount of zero's in received data
	uint8_t receive;						//Receive variable
	uint8_t player;							//Player value (1 or 2)
	uint16_t xTank;							//X value of tank to be send
	uint8_t yTank;							//Y value of tank to be send
	uint8_t bullet;							//Bullet fired or not to be send
	uint8_t health;							//Health to be send
	TankClass *ownTank;						//Pointer to own tank
	TankClass *opponentTank;				//Pointer to opponent tank			
	ScreensClass *screen;					//Pointer to screen
	uint8_t refreshGame;					//Variable to refresh the game

	void encodeData();						//Encode data into 0's and 1's and put them into bufferSend
	void sendStartData();					//Send start bits
	void sendData();						//Send main and inverse data
	void sendEndData();						//Send end bits
	void receiveData();						//Receive data and put them as 0's and 1's into bufferReceive
	void decodeXTank();						//Decode X position of tank and update value in tank class
	void decodeYTank();						//Decode X position of tank and update value in tank class
	void decodeBullet();					//Decode bullet value of opponent and update value in tank class
	void decodeHealth();					//Decode health of opponent and update value in tank class
public:
	void init(char *mainBufferSend, char *inverseBufferSend, char *mainBufferReceive, char *inverseBufferReceive, TankClass *tankLeft, TankClass *tankRight, ScreensClass *screen, volatile uint16_t *counterSend, volatile uint16_t *counterReceive, volatile uint16_t *connectionTime, volatile uint8_t *inputChange, uint8_t player);	//Initialize IR	void encodeData(uint8_t data);
	void run();
	void setRefreshGame(uint8_t value);		//Set value to refreshGame
	uint8_t getRefreshGame();				//Get value of refreshGame
	uint8_t checkData();					//Check if data is correctly received
};

extern IRClass IR;

#endif