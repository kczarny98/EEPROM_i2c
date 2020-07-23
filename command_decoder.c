#include "command_decoder.h"
#include "string.h"

#define NULL 0
#define LAST_TETRAD 0x0F
#define MAX_TOKEN_NR 3
#define MAX_KEYWORD_STRING_LTH 15
#define MAX_KEYWORD_NR 9
#define DELIMITER_CHARACTER ' '

enum TokensState {TOKEN, DELIMITER};
enum Result {OK, ERROR};

struct Keyword {
	enum KeywordCode eCode;
	char cString[MAX_KEYWORD_STRING_LTH + 1];
};

struct Keyword asKeywordList[MAX_KEYWORD_NR] = {
	{CL, "callib"},
	{GT, "goto"},
	{SP, "step"},
	{CC, "calc"},
	{ID, "id"},
	{SET, "spi_port_set"},
	{GET, "spi_port_get"},
	{WRITE, "memwr"},
	{READ, "memrd"}
};

struct Token asToken[MAX_TOKEN_NR];
unsigned char ucTokenNr = 0;

void UIntToHexStr(unsigned int uiValue, char pcStr[]) {

	unsigned char ucTetradCounter;
	unsigned char ucCurrentTetrad;

	pcStr[0] = '0';
	pcStr[1] = 'x';
	pcStr[6] = NULL;
	for (ucTetradCounter = 0; ucTetradCounter < 4; ucTetradCounter++) {
		ucCurrentTetrad = (uiValue >> (ucTetradCounter * 4)) & LAST_TETRAD;
		if (ucCurrentTetrad < 10) {
			pcStr[5 - ucTetradCounter] = '0' + ucCurrentTetrad;
		}
		else {
			pcStr[5 - ucTetradCounter] = 'A' + (ucCurrentTetrad - 10);
		}
	}
}

enum Result eHexStringToUInt(char pcStr[], unsigned int *puiValue) {

	unsigned char ucCharacterCounter;
	unsigned char ucCurrentCharacter;

	*puiValue = 0;

	if (pcStr[0] != '0' || pcStr[1] != 'x' || pcStr[2] == NULL) {
		return ERROR;
	}
	for (ucCharacterCounter = 2; ucCharacterCounter < 6; ucCharacterCounter++) {
		ucCurrentCharacter = pcStr[ucCharacterCounter];
		if (ucCurrentCharacter == NULL) {
			return OK;
		}
		*puiValue = *puiValue << 4;
		if (ucCurrentCharacter <= '9' && ucCurrentCharacter >= '0') {
			ucCurrentCharacter = ucCurrentCharacter - '0';
		}
		else if (ucCurrentCharacter <= 'F' && ucCurrentCharacter >= 'A') {
			ucCurrentCharacter = ucCurrentCharacter - 'A' + 10;
		}
		else {
			return ERROR;
		}
		*puiValue = *puiValue | ucCurrentCharacter;
	}
	if (pcStr[ucCharacterCounter] == NULL) {
		return OK;
	}
	else {
		return ERROR;
	}
}

void AppendUIntToString (unsigned int uiValue, char pcDestinationStr[]) {

	unsigned char ucCharacterCounter;

	for (ucCharacterCounter = 0; pcDestinationStr[ucCharacterCounter] != NULL; ucCharacterCounter++) {}
	UIntToHexStr(uiValue, pcDestinationStr + ucCharacterCounter);
}

unsigned char ucFindTokensInString(char *pcString) {

	enum TokensState eCurrentTokenType = DELIMITER;
	unsigned char ucCharacterCounter;
	char cCurrentCharacter;

	ucTokenNr = 0;

	for (ucCharacterCounter = 0; ;ucCharacterCounter++) {
		cCurrentCharacter = pcString[ucCharacterCounter];
		switch (eCurrentTokenType) {
			case TOKEN:
				if (ucTokenNr == MAX_TOKEN_NR) {
					return ucTokenNr;
				}
				else if (cCurrentCharacter == DELIMITER_CHARACTER) {
					eCurrentTokenType = DELIMITER;
				}
				else if (cCurrentCharacter == NULL) {
					return ucTokenNr;
				}
				else {
					eCurrentTokenType = TOKEN;
				}
				break;
			case DELIMITER:
				if (cCurrentCharacter == NULL) {
					return ucTokenNr;
				}
				else if (cCurrentCharacter != DELIMITER_CHARACTER) {
					eCurrentTokenType = TOKEN;
					asToken[ucTokenNr].uValue.pcString = pcString + ucCharacterCounter;
					ucTokenNr++;
				}
				else {
					eCurrentTokenType = DELIMITER;
				}
				break;
		}
	}
}

enum Result eStringToKeyword (char pcStr[], enum KeywordCode *peKeywordCode) {

	unsigned char ucKeywordIndex;

	for (ucKeywordIndex = 0; ucKeywordIndex < MAX_KEYWORD_NR; ucKeywordIndex++) {
		if (eCompareString(pcStr, asKeywordList[ucKeywordIndex].cString) == EQUAL) {
			*peKeywordCode = asKeywordList[ucKeywordIndex].eCode;
			return OK;
		}
	}
	return ERROR;
}

void DecodeTokens(void) {

	unsigned char ucTokenIndex;
	enum KeywordCode eCurrentKeywordCode;
	unsigned int uiCurrentValue;
	struct Token *psCurrentToken;

	for (ucTokenIndex = 0; ucTokenIndex < ucTokenNr; ucTokenIndex++) {
		psCurrentToken = &asToken[ucTokenIndex];
		if (eStringToKeyword(psCurrentToken->uValue.pcString, &eCurrentKeywordCode) == OK) {
			psCurrentToken->eType = KEYWORD;
			psCurrentToken->uValue.eKeyword = eCurrentKeywordCode;
		}
		else if (eHexStringToUInt(psCurrentToken->uValue.pcString, &uiCurrentValue) == OK) {
			psCurrentToken->eType = NUMBER;
			psCurrentToken->uValue.uiNumber = uiCurrentValue;
		}
		else {
			psCurrentToken->eType = STRING;
		}
	}
}

void DecodeMsg(char *pcString) {
	ucFindTokensInString(pcString);
	ReplaceCharactersInString(pcString, DELIMITER_CHARACTER, NULL);
	DecodeTokens();
}
