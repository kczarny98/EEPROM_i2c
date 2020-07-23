#include "LPC21xx.h"
#include "i2c.h"

#define SCL_I2C (1<<4)
#define SDA_I2C (1<<6)

#define AA (1<<2)
#define SI (1<<3)
#define STO (1<<4)
#define STA (1<<5)
#define I2EN (1<<6)

#define VIC_I2C_CHANNEL_NR 9
#define mIRQ_SLOT_ENABLE 0x00000020

#define I2C_STATUS_START 								0x08
#define I2C_STATUS_RESTART							0x10
#define I2C_STATUS_ADDRESS_SENT_ACK 		0x18
#define I2C_STATUS_ADDRESS_SENT_NACK 		0x20
#define I2C_STATUS_DATA_SENT_ACK 				0x28
#define I2C_STATUS_ADDRESS_RECEIVE_ACK 	0x40
#define I2C_STATUS_ADDRESS_RECEIVE_NACK 0x48
#define I2C_STATUS_DATA_RECEIVED_ACK 		0x50
#define I2C_STATUS_DATA_RECEIVED_NACK 	0x58
#define I2C_STATUS_IDLE 								0xF8

struct I2C_Params sI2C_Params;
unsigned char ucByteCounter = 0;

__irq void I2C_Interrupt(void){
	
		unsigned char uiStatus = I2STAT;
		
		switch(uiStatus)
		{
			case I2C_STATUS_START:
			case I2C_STATUS_RESTART:
					I2DAT = ((sI2C_Params.ucSlaveAddress << 1) | ((sI2C_Params.eI2CTransmisionMode == RX ) ? 1 : 0));
					I2CONCLR = STA;
					break;
			case I2C_STATUS_ADDRESS_SENT_ACK:
					I2DAT = sI2C_Params.pucBytesForTx[ucByteCounter++];
					break;
			case I2C_STATUS_ADDRESS_SENT_NACK:
					I2DAT = ((sI2C_Params.ucSlaveAddress << 1) | ((sI2C_Params.eI2CTransmisionMode == RX ) ? 1 : 0));
					break;
			case I2C_STATUS_ADDRESS_RECEIVE_ACK:
					break;
			case I2C_STATUS_ADDRESS_RECEIVE_NACK:
					I2CONSET = STO;
					sI2C_Params.ucDone = 1;
					break;
			case I2C_STATUS_DATA_SENT_ACK:
					
					if (ucByteCounter < sI2C_Params.ucNrOfBytesForTx)
					{
						I2DAT = sI2C_Params.pucBytesForTx[ucByteCounter++];
					}

					else
					{
						if (sI2C_Params.eI2CTransmisionMode == TX)
						{
								I2CONSET = STO;
								sI2C_Params.ucDone = 1;
						}
						else if (sI2C_Params.eI2CTransmisionMode == TX_AFTER_RX)
						{
								sI2C_Params.eI2CTransmisionMode = RX;
								ucByteCounter = 0;
								I2CONSET = STA; // stop i start (restart) ?
						}
					}
					break;
			case I2C_STATUS_DATA_RECEIVED_ACK:
					if (ucByteCounter < sI2C_Params.ucNrOfBytesForRx)
					{
						sI2C_Params.pucBytesForRx[ucByteCounter++] = I2DAT;
					}
					else
					{
						I2CONSET = STO;
						sI2C_Params.ucDone = 1;
					}
					break;
			case I2C_STATUS_DATA_RECEIVED_NACK:
					sI2C_Params.pucBytesForRx[ucByteCounter++] = I2DAT;
					I2CONSET = STO;
					sI2C_Params.ucDone = 1;
					break;
			default:
					break;
		}
		
		I2CONCLR = SI;
		VICVectAddr = 0x00;
}

void I2C_ExecuteTransaction()
{
		ucByteCounter = 0;
		I2CONCLR = SI;
		I2CONSET = STA;
		sI2C_Params.ucDone = 0;
}

void I2C_Init(void) 
{
		/* I2C Pin Connect Block */
		PINSEL0 |= (SCL_I2C | SDA_I2C);
	
		/* Clear I2C status register */
		I2CONCLR = (AA | SI | STO | STA | I2EN);
	
		/* Enable I2C bus */
		I2CONSET = I2EN;
	
		/* Set proper frequency */
		I2SCLH = 0x80;
		I2SCLL = 0x80;
	
		/* Configure interrupts (channel 9, slot 1) */
		VICIntEnable |= (1 << VIC_I2C_CHANNEL_NR);
		VICVectCntl1  = mIRQ_SLOT_ENABLE | VIC_I2C_CHANNEL_NR;
		VICVectAddr1  = (unsigned long)I2C_Interrupt;
}
