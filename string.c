#include "string.h"

#define NULL 0

void CopyString(char pcSource[], char pcDestination[]) {
	unsigned char ucCounter;
	for (ucCounter = 0; pcSource[ucCounter] != NULL; ucCounter++) {
		pcDestination[ucCounter] = pcSource[ucCounter];
	}
	pcDestination[ucCounter] = NULL;
}

enum CompResult eCompareString(char pcStr1[], char pcStr2[]) {
	unsigned char ucCounter;
	for (ucCounter = 0; pcStr1[ucCounter] != NULL; ucCounter++) {
		if (pcStr1[ucCounter] != pcStr2[ucCounter])
			return DIFFERENT;
	}
	if (pcStr1[ucCounter] != pcStr2[ucCounter])
		return DIFFERENT;
	else
		return EQUAL;
}

void AppendString(char pcSourceStr[], char pcDestinationStr[]) {
	unsigned char ucCounter;
	for (ucCounter = 0; pcDestinationStr[ucCounter] != NULL; ucCounter++) {
	}
	CopyString(pcSourceStr, pcDestinationStr + ucCounter);
}

void ReplaceCharactersInString(char pcString[], char cOldChar, char cNewChar) {
	unsigned char ucCounter;
	for (ucCounter = 0; pcString[ucCounter] != NULL; ucCounter++) {
		if (pcString[ucCounter] == cOldChar)
			pcString[ucCounter] = cNewChar;
	}
}
