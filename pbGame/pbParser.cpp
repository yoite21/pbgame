#include <string>

#include "pbParser.h"

void pbParser::playerIdAndNumberToStr(char* str, int len, int playerId, int playerNumber)
{
	sprintf_s(str, len, "%d %d", playerId, playerNumber);
	
}

void pbParser::strToPlayerIdAndNumber(const char* str, int &playerId, int &playerNumber)
{
	sscanf_s(str, "%d %d", playerId, playerNumber);
}