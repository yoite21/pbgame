#pragma once

class pbParser {
public:

	static void playerIdAndNumberToStr(char* str, int len, int playerId, int playerNumber);
	static void strToPlayerIdAndNumber(const char* str, int &playerId, int &playerNumber);

};