#include "mc24lc64.h"
#include "i2c.h"

#define NULL 0
#define MC24LC64_ADDR 0x50
#define ADDR_HIGH_BITS ((1<<8) | (1<<9) | (1<<10) | (1<<11) | (1<<12))
#define ADDR_LOW_BITS 255

extern unsigned char ucMC24LC64_Input;
extern struct I2C_Params sI2C_Params;

void MC24LC64_ByteWrite(unsigned int uiWordAddress, unsigned char ucData)
{
	unsigned char aucBytesForTx[3];
	
	aucBytesForTx[0] = ((uiWordAddress & ADDR_HIGH_BITS) >> 8);
	aucBytesForTx[1] = (uiWordAddress & ADDR_LOW_BITS);
	aucBytesForTx[2] = ucData;

	sI2C_Params.eI2CTransmisionMode = TX;
	sI2C_Params.ucSlaveAddress = MC24LC64_ADDR;
	sI2C_Params.ucNrOfBytesForTx = 3;
	sI2C_Params.pucBytesForTx = aucBytesForTx;
	sI2C_Params.pucBytesForRx = 0;
	
	I2C_ExecuteTransaction();
	
}

void MC24LC64_RandomRead(unsigned int uiWordAddress)
{

	unsigned char aucBytesForTx[3];
	unsigned char aucBytesForRx[2];
	
	aucBytesForTx[0] = ((uiWordAddress & ADDR_HIGH_BITS) >> 8);
	aucBytesForTx[1] = (uiWordAddress & ADDR_LOW_BITS);

	sI2C_Params.eI2CTransmisionMode = RX_AFTER_TX;
	sI2C_Params.ucSlaveAddress = MC24LC64_ADDR;
	sI2C_Params.ucNrOfBytesForTx = 2;
	sI2C_Params.pucBytesForTx = aucBytesForTx;
	sI2C_Params.ucNrOfBytesForRx = 2; // moze 2? albo 4!!!
	sI2C_Params.pucBytesForRx = aucBytesForRx;
	
	I2C_ExecuteTransaction();
	
}
