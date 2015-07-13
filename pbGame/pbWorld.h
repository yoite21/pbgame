#pragma once
#include <vector>
#include <string>
#include <Windows.h>

using std::string;
using std::vector;

#include "pbTerrain.h"

#include "pbGhost.h"

#include "pbPlayer.h"
class pbWorld
{
public:
	void render();
	int loadMap();
	
	void genInitialGhostPosition();
	void genInitialPlayerPosition();

	string getPositionInfo(bool isHost);

	
	void loadModel();
	void advanceModels();
	

	// get player and ghost position with string
	void updatePlayersPosition(vector<string> strVector);

	void cameraMove(float horizonMove, float verticalMove);
	void playerMove(int x, int y);
	void ghostMove();

	void tryCatchPlayer();

	void addHostPlayer();
	void addSinglePlayer();
	void addClientPlayer(int newPlayerId);
	void disconnectClientPlayer(int disconnectPlayerId);

	void setMyPlayerId(int id);


public:
	pbWorld();
	~pbWorld();
private:
	vector<pbVecf> getObjectVector();
	int getRandomLivePlayerId();

private:
	pbTerrain        mTerrain;
	vector<pbPlayer> mPlayerVector;
	vector<pbGhost>  mGhostVector;
	vector<string>   mPreRecvBuffer;
	
	
	int mMyPlayerId;
	bool mInitPosition;	// true = inited
	
	static const int PLAYER_ALL_STRING_LEN;
	static const int PLAYER_STATE_STRING_LEN;
	static const int GHOST_ALL_STRING_LEN;
	static const int SINGLE_PLAYER_NUM;
};