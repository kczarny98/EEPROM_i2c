#include "pcf.h"
#include "i2c.h"

#define NULL 0
#define PCF8574T_ADDR 0x20

extern unsigned char ucPCF8574_Input;
extern struct I2C_Params sI2C_Params;

void PCF8574_Write(unsigned char ucData)
{
	unsigned char aucBytesForTx[2];
	
	aucBytesForTx[0] = ucData;

	sI2C_Params.eI2CTransmisionMode = TX;
	sI2C_Params.ucSlaveAddress = PCF8574T_ADDR;
	sI2C_Params.ucNrOfBytesForTx = 1;
	sI2C_Params.pucBytesForTx = aucBytesForTx;	
	I2C_ExecuteTransaction();
}

void PCF8574_Read(void)
{
	sI2C_Params.eI2CTransmisionMode = RX;
	sI2C_Params.ucSlaveAddress = PCF8574T_ADDR;
	sI2C_Params.ucNrOfBytesForRx = 1;
	sI2C_Params.pucBytesForRx = &ucPCF8574_Input;	
	I2C_ExecuteTransaction();
}
