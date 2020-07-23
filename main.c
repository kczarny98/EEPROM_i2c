#include "i2c.h"
#include "mc24lc64.h"
#include "command_decoder.h"
#include "string.h"
#include "uart.h"
#include "pcf.h"
#include "led.h"

#define NULL 0
#define TIMER 5400

extern struct I2C_Params sI2C_Params;
unsigned char ucPCF8574_Input = 0;
unsigned char ucMC24LC64_Input = 0;

void Delay(int k) {
	int i;
	for (i=0; i<TIMER*k; i++) {
	}
}

int main()
{
	unsigned int i = 0;
	
	unsigned char fReadCommandReceived = 0;
	unsigned char fIdReceived = 0;
	unsigned char fUnknownCommand = 0;
	char pcMessage[TRANSMITER_SIZE] = "";
	char pcMessageReceived[RECIEVER_SIZE] = "";

	unsigned int uiValueStored = 0;
	
	I2C_Init();
	UART_InitWithInt(9600);

	/*	
	while(1)
	{
		Delay(100);
		//PCF8574_Write(0xFF);
		//PCF8574_Read();
		//MC24LC64_ByteWrite(0xFFFF, 0xFF);
		MC24LC64_RandomRead(0xFFFF);
		while (!sI2C_Params.ucDone) {}
	}
	*/
	
	while(1)
	{
		if (eReciever_GetStatus() == READY) {
			Reciever_GetStringCopy(pcMessageReceived);
			DecodeMsg(pcMessageReceived);
			if (asToken[0].eType == KEYWORD) {
				switch(asToken[0].uValue.eKeyword) {
					case ID:
						fIdReceived = 1;
					break;
					case WRITE:
						MC24LC64_ByteWrite(asToken[1].uValue.uiNumber, asToken[2].uValue.uiNumber);
						while (!sI2C_Params.ucDone) {}
					break;
					case READ:
						fReadCommandReceived = 1;
						uiValueStored = asToken[1].uValue.uiNumber;
					break;
					default:
						fUnknownCommand = 1;
					break;
				}	
			}
			else {
				fUnknownCommand = 1;
			}
		}
		
		if (Transmiter_GetStatus() == FREE) {
			if (fReadCommandReceived == 1) {
				fReadCommandReceived = 0;
				MC24LC64_RandomRead(uiValueStored);
				while (!sI2C_Params.ucDone) {}
				CopyString("value stored: ", pcMessage);
				AppendUIntToString(sI2C_Params.pucBytesForRx[1], pcMessage);
				Transmiter_SendString(pcMessage);
			}
			else if (fIdReceived == 1) {
				fIdReceived = 0;
				Transmiter_SendString("id LPC2132");
			}
			else if (fUnknownCommand == 1) {
				fUnknownCommand = 0;
				Transmiter_SendString("unknowncommand");
			}
		}
	}
}
