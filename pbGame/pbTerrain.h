#pragma once

#include "glut.h"
#include "pbDef.h"

#include <vector>

using std::vector;

typedef struct _wallInfo {
	int textureId;
	int startX;
	int startZ;
	int endX;
	int endZ;
	float distance;

	_wallInfo (int _startX, int _startZ, int _endX, int _endZ)
	{
		startX = _startX;
		startZ = _startZ;
		endX = _endX;
		endZ = _endZ;
	}

	_wallInfo (int _textureId, int _startX, int _startZ, int _endX, int _endZ)
	{
		textureId = _textureId;
		startX = _startX;
		startZ = _startZ;
		endX = _endX;
		endZ = _endZ;

		distance = (float)sqrt((float)(pow((float)(startX-endX),2)+pow((float)(startZ-endZ),2)));
	}

} WallInfo;

class pbTerrain {
public:
	void loadMapFromFile();
	void makeDisplayList();
	void render();

	pbVecf getAblePosition(vector<pbVecf> objectPoint, float objectDistantRange);

	bool isWallCollision(pbVecf nextPosition, pbVecf curPosition);

	int getTerrainWidth();
	int getTerrainHeight();
	void genInitPosition();
public:
	pbTerrain();
	~pbTerrain();

private:
	bool isPossiblePosition(pbVecf point, vector<pbVecf> objectPoint, float range);

	bool isInRange(WallInfo wall, pbVecf point);
	bool isInCircle(pbVecf point, pbVecf circleMiddlePoint, float circleRadius);
	bool isSingleWallCollision(pbVecf position1, pbVecf position2, WallInfo wall);
	bool isLineCross(pbVecf line0StartPoint, pbVecf line0EndPoint, pbVecf line1StartPoint, pbVecf line1EndPoint); // 선분 교차 체크
private:
	vector<pbVecf> mInitPosition;
	int mTerrainWidth;
	int mTerrainHeight;

	GLuint mGroundTexture;
	GLuint* mWallTexture;

	int mWallTextureNumber;

	GLuint* mTerrainList;

	vector<WallInfo> mWallInfoVector;

	static const float CRASH_RANGE;
	static const int GENERATE_INIT_POSITION_WIDTH_NUM;
	static const int GENERATE_INIT_POSITION_HEIGHT_NUM;
};