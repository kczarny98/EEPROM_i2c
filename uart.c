#include "LPC21xx.h"
#include "string.h"

#define NULL 0

#define mPIN_FUNCTION_RXD_UART0 									 0x00000004
#define mPIN_FUNCTION_TXD_UART0 									 0x00000001

#define mDIVISOR_LATCH_ACCES_BIT                   0x00000080
#define m8BIT_UART_WORD_LENGTH                     0x00000003

#define mRX_DATA_AVALIABLE_INTERRUPT_ENABLE        0x00000001
#define mTHRE_INTERRUPT_ENABLE                     0x00000002

#define mINTERRUPT_PENDING_IDETIFICATION_BITFIELD  0x0000000F
#define mTHRE_INTERRUPT_PENDING                    0x00000002
#define mRX_DATA_AVALIABLE_INTERRUPT_PENDING       0x00000004

#define VIC_UART0_CHANNEL_NR  6
#define VIC_UART1_CHANNEL_NR  7

#define mIRQ_SLOT_ENABLE                           0x00000020

#define RECIEVER_SIZE 30
#define TRANSMITER_SIZE 30
#define TERMINATOR '\r'

char cOdebranyZnak;
char cWysylanyZnak;

enum eRecieverStatus {EMPTY, READY, OVERFLOW};
enum eTransmiterStatus {FREE, BUSY};

struct RecieverBuffer {
	char cData[RECIEVER_SIZE];
	unsigned char ucCharCtr;
	enum eRecieverStatus eStatus;
};

struct TransmiterBuffer {
	char cData[TRANSMITER_SIZE];
	enum eTransmiterStatus eStatus;
	unsigned char fLastCharacter;
	unsigned char ucCharCtr;
};

struct RecieverBuffer sRxBuffer = {"", 0, EMPTY};
struct TransmiterBuffer sTransmiterBuffer;

char Transmiter_GetCharacterFromBuffer(void) {
	if (sTransmiterBuffer.fLastCharacter == 1) {
		sTransmiterBuffer.eStatus = FREE;
		return NULL;
	}
	else if (sTransmiterBuffer.cData[sTransmiterBuffer.ucCharCtr] != NULL) {
		return sTransmiterBuffer.cData[sTransmiterBuffer.ucCharCtr++];
	}
	else {
		sTransmiterBuffer.fLastCharacter = 1;
		return TERMINATOR;
	}
}

void Transmiter_SendString(char cString[]) {
	CopyString(cString, sTransmiterBuffer.cData);
	U0THR = sTransmiterBuffer.cData[0];
	sTransmiterBuffer.eStatus = BUSY;
	sTransmiterBuffer.ucCharCtr = 1;
	sTransmiterBuffer.fLastCharacter = 0;
}

enum eTransmiterStatus Transmiter_GetStatus(void) {
	return sTransmiterBuffer.eStatus;
}

void Reciever_PutCharacterToBuffer(char cCharacter) {
	if (sRxBuffer.eStatus != EMPTY) {
		return;
	}
	else if (sRxBuffer.ucCharCtr == RECIEVER_SIZE) {
		sRxBuffer.eStatus = OVERFLOW;
	}
	else if (cCharacter == TERMINATOR) {
		sRxBuffer.cData[sRxBuffer.ucCharCtr] = NULL;
		sRxBuffer.eStatus = READY;
		sRxBuffer.ucCharCtr = 0;
	}
	else {
		sRxBuffer.cData[sRxBuffer.ucCharCtr] = cCharacter;
		sRxBuffer.ucCharCtr++;
	}
}

enum eRecieverStatus eReciever_GetStatus(void) {
	return sRxBuffer.eStatus;
}

void Reciever_GetStringCopy(char *ucDestination) {
	CopyString(sRxBuffer.cData, ucDestination);
	sRxBuffer.eStatus = EMPTY;
}


__irq void UART0_Interrupt (void) {
   
   unsigned int uiCopyOfU0IIR=U0IIR; 

   if      ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mRX_DATA_AVALIABLE_INTERRUPT_PENDING) 
   {
     cOdebranyZnak = U0RBR;
		 Reciever_PutCharacterToBuffer(cOdebranyZnak);
   } 
   
   if ((uiCopyOfU0IIR & mINTERRUPT_PENDING_IDETIFICATION_BITFIELD) == mTHRE_INTERRUPT_PENDING)              
   {
     cWysylanyZnak = Transmiter_GetCharacterFromBuffer();
		 if (cWysylanyZnak != NULL) {
				U0THR = cWysylanyZnak;
			}
   }

   VICVectAddr = 0; 
}

void UART_InitWithInt(unsigned int uiBaudRate){

   PINSEL0 = PINSEL0 | mPIN_FUNCTION_RXD_UART0 | mPIN_FUNCTION_TXD_UART0;                 
   U0LCR  |= m8BIT_UART_WORD_LENGTH | mDIVISOR_LATCH_ACCES_BIT; 
	 U0DLL   = ((15000000)/16)/(uiBaudRate);                      
   U0LCR  &= (~mDIVISOR_LATCH_ACCES_BIT);                       
   U0IER  = U0IER | mRX_DATA_AVALIABLE_INTERRUPT_ENABLE | mTHRE_INTERRUPT_ENABLE;               

   VICVectAddr2  = (unsigned long) UART0_Interrupt;             
   VICVectCntl2  = mIRQ_SLOT_ENABLE | VIC_UART0_CHANNEL_NR;     
   VICIntEnable |= (0x1 << VIC_UART0_CHANNEL_NR);              
}
