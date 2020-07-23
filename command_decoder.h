#define MAX_TOKEN_NR 3

void DecodeMsg(char *pcString);
void AppendUIntToString (unsigned int uiValue, char pcDestinationStr[]);
void UIntToHexStr(unsigned int uiValue, char pcStr[]);
enum Result eHexStringToUInt(char pcStr[], unsigned int *puiValue);
enum TokenType {KEYWORD, NUMBER, STRING};
enum KeywordCode {CL, GT, SP, CC, ID, SET, GET, WRITE, READ};

union TokenValue {
	enum KeywordCode eKeyword;
	unsigned int uiNumber;
	char *pcString;
};

struct Token {
	enum TokenType eType;
	union TokenValue uValue;
};

extern struct Token asToken[MAX_TOKEN_NR];
