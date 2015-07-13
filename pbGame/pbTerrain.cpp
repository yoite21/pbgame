#include <stdio.h>
#include <fstream>



#include "imageloader.h"


using std::fstream;

#include "pbTerrain.h"

const float pbTerrain::CRASH_RANGE = 3.0f;
const int pbTerrain::GENERATE_INIT_POSITION_WIDTH_NUM = 16;
const int pbTerrain::GENERATE_INIT_POSITION_HEIGHT_NUM = 16;


void clearLastString(char* str, int len);

void pbTerrain::loadMapFromFile()
{
	FILE* fp;

	fopen_s(&fp,"./resource/mapinfo.txt", "r");

	char str[256];
	// load width, height
	fscanf_s(fp, "%d %d\n", &mTerrainWidth, &mTerrainHeight);

	fgets(str, 256, fp);
	clearLastString(str, 256);

	// load ground texture
	Image* image;
	image = loadBMP(str);
	mGroundTexture = loadTexture(image);

	glBindTexture(GL_TEXTURE_2D,mGroundTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	delete image;

	// load initial generate able position
	pbVecf initPositionPoint;
	fscanf_s(fp, "%f %f\n", &initPositionPoint.x, & initPositionPoint.z );
	mInitPosition.push_back(initPositionPoint);

	// load wall texture 
	fscanf_s(fp, "%d\n", &mWallTextureNumber);
	mWallTexture = new GLuint[mWallTextureNumber];
	for(int i = 0; i < mWallTextureNumber; i++)
	{
		fgets(str, 256, fp);
		clearLastString(str, 256);

		image = loadBMP(str);
		mWallTexture[i] = loadTexture(image);

		glBindTexture(GL_TEXTURE_2D,mWallTexture[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		delete image;
	}

	// load wall info
	int textureId;
	int startX;
	int startZ;
	int endX;
	int endZ;

	while(!feof(fp))
	{
		fscanf_s(fp, "%d %d %d %d %d\n", &textureId, &startX, &startZ, &endX, &endZ);

		mWallInfoVector.push_back(WallInfo(textureId, startX, startZ, endX, endZ));
	}

	fclose(fp);
}


void pbTerrain::makeDisplayList()
{
	// make opengl display list

	mTerrainList = new GLuint[1+mWallTextureNumber];
	mTerrainList[0] = glGenLists(1);
	
	// ground
	glNewList(mTerrainList[0], GL_COMPILE);

	glBegin(GL_QUADS);
	glBindTexture(GL_TEXTURE_2D, mGroundTexture);

	glTexCoord2f(0,0);
	glVertex3f(0,0,0);
	glTexCoord2f(1,0);
	glVertex3f((float) mTerrainWidth, 0, 0);
	glTexCoord2f(1,1);
	glVertex3f((float) mTerrainWidth, 0, (float) mTerrainHeight);
	glTexCoord2f(0,1);
	glVertex3f(0,0,(float) mTerrainHeight);

	glTexCoord2f(0,0);
	glVertex3f(0,50,0);
	glTexCoord2f(1,0);
	glVertex3f((float) mTerrainWidth, 50, 0);
	glTexCoord2f(1,1);
	glVertex3f((float) mTerrainWidth, 50, (float) mTerrainHeight);
	glTexCoord2f(0,1);
	glVertex3f(0,50,(float) mTerrainHeight);

	glEnd();
	glEndList();

	// wall
	for(int i = 0; i < mWallTextureNumber; i++)
	{
		mTerrainList[i+1] = glGenLists(1);
		glNewList(mTerrainList[i+1], GL_COMPILE);

		glBegin(GL_QUADS);
		for(unsigned int j = 0; j < mWallInfoVector.size(); j++)
		{
			if( mWallInfoVector[j].textureId != i )
				continue;
			glTexCoord2f(1,0);
			glVertex3f(mWallInfoVector[j].startX,0,mWallInfoVector[j].startZ);
			glTexCoord2f(1,1);
			glVertex3f(mWallInfoVector[j].startX,50,mWallInfoVector[j].startZ);
			glTexCoord2f(0,1);
			glVertex3f(mWallInfoVector[j].endX,50,mWallInfoVector[j].endZ);
			glTexCoord2f(0,0);
			glVertex3f(mWallInfoVector[j].endX,0,mWallInfoVector[j].endZ);
		}
		glEnd();
		glEndList();
	}
}

void pbTerrain::render()
{

	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	for(int i = 0; i < 1+mWallTextureNumber; i++)
	{
		if(i == 0)
			glBindTexture(GL_TEXTURE_2D, mGroundTexture);
		else
			glBindTexture(GL_TEXTURE_2D, mWallTexture[i-1]);
		glCallList(mTerrainList[i]);
	}
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

}


pbVecf pbTerrain::getAblePosition(vector<pbVecf> objectPoint, float objectDistantRange)
{
	
	int initPositionNum = mInitPosition.size();
	int randIndex;
	do
	{
		randIndex = rand() % (initPositionNum-1) + 1;
	} while ( !isPossiblePosition(mInitPosition[randIndex], objectPoint, objectDistantRange ) );

	return mInitPosition[randIndex];
}


bool pbTerrain::isWallCollision(pbVecf nextPosition, pbVecf curPosition)
{
	int wallNum = mWallInfoVector.size();
	for(int i = 0; i < wallNum; i++)
	{
		// 1) 이동할 위치가 각 모서리 원 안에 없을것	
		if ( isInCircle(nextPosition, pbVecf(mWallInfoVector[i].startX, 0 , mWallInfoVector[i].startZ), CRASH_RANGE) )
			return true;
		if ( isInCircle(nextPosition, pbVecf(mWallInfoVector[i].endX, 0 , mWallInfoVector[i].endZ), CRASH_RANGE) )
			return true;
		
		// 2) 현재 위치가 해당 벽의 충돌체크 범위 안에 있는지 확인
		if ( !isInRange(mWallInfoVector[i], curPosition ) )
			continue;
		
		float sin = (mWallInfoVector[i].startZ - mWallInfoVector[i].endZ) / mWallInfoVector[i].distance;
		float cos = (mWallInfoVector[i].startX - mWallInfoVector[i].endX) / mWallInfoVector[i].distance;

		// 3) 현재위치와 이동할 위치가 일정거리 떨어진 벽을 넘어가는지 체크 (ccw 알고리즘 사용)
		if ( isSingleWallCollision(nextPosition, curPosition,
 			WallInfo(mWallInfoVector[i].startX - CRASH_RANGE*sin, mWallInfoVector[i].startZ + CRASH_RANGE*cos, mWallInfoVector[i].endX - CRASH_RANGE*sin, mWallInfoVector[i].endZ + CRASH_RANGE*cos) ))
			return true;

		if ( isSingleWallCollision(nextPosition, curPosition,
			WallInfo(mWallInfoVector[i].startX + CRASH_RANGE*sin, mWallInfoVector[i].startZ - CRASH_RANGE*cos, mWallInfoVector[i].endX + CRASH_RANGE*sin, mWallInfoVector[i].endZ - CRASH_RANGE*cos) ))
			return true;
	}

	return false;
	
	
	//
	// 기존 방식
	//
	/*
	float curA;
	float curB;
	float curC;

	// ax + by = c
	if( unitVector.x == 0 )
	{
		curA = 1;
		curB = 0;
		curC = curPosition.x;
	}
	else if( unitVector.z == 0)
	{
		curA = 0;
		curB = 1;
		curC = curPosition.z;
	}
	else
	{
		curA = - unitVector.z / unitVector.x;
		curB = 1;
		curC = curA * curPosition.x + curPosition.z;
	}
		
	float wallA;
	float wallB;
	float wallC;
	
	int wallInfoVectorSize = wallInfoVector.size();
	for(int i = 0; i < wallInfoVectorSize; i++)
	{
		// dx + ey = f
		if( wallInfoVector[i].startX == wallInfoVector[i].endX )
		{
			wallA = 1;
			wallB = 0;
			wallC = wallInfoVector[i].startX;
		}
		else if( wallInfoVector[i].startZ == wallInfoVector[i].endZ )
		{
			wallA = 0;
			wallB = 1;
			wallC = wallInfoVector[i].startZ;
		}
		else
		{
			wallA = - ( wallInfoVector[i].startZ - wallInfoVector[i].endZ) / (wallInfoVector[i].startX - wallInfoVector[i].endX);
			wallB = 1;
			wallC = wallA * wallInfoVector[i].startX + wallInfoVector[i].startZ;
		}

		

		if( wallA == 0 && curA == 0)
		{
			if( wallB * curC != wallC * curB)
				continue;
			else
			{
				if(isInLIneCollision(wallInfoVector[i].startZ, wallInfoVector[i].endZ, curPosition.z, curPosition.z + unitVector.z))
					return true;
				else
					continue;
			}
		}
		else if( wallB == 0 && curB == 0)
		{
			if(wallA*curC != wallC*curA)
				continue;
			else
			{
				if(isInLIneCollision(wallInfoVector[i].startX, wallInfoVector[i].endX, curPosition.x, curPosition.x + unitVector.x))
					return true;
				else
					continue;
			}
		}
		else
		{
			float x = (curB*wallC - curC*wallB) / (wallA*curB - curA*wallB);
			float z = (wallA*curC - curA*wallC) / (wallA*curB - curA*wallB);
			if(isInLIneCollision(wallInfoVector[i].startX, wallInfoVector[i].endX, x, x) &&
				isInLIneCollision(wallInfoVector[i].startZ, wallInfoVector[i].endZ, z, z) &&
				isInLIneCollision(curPosition.x, curPosition.x + unitVector.x,x,x)&&
				isInLIneCollision(curPosition.z, curPosition.z + unitVector.z,z,z))
				return true;
			else
				continue;
		}

	}
	return false;

	*/
}



int pbTerrain::getTerrainWidth()
{
	return mTerrainWidth;
}

int pbTerrain::getTerrainHeight()
{
	return mTerrainHeight;
}

void pbTerrain::genInitPosition()
{
	vector<int> indexQueue;
	bool positionFlag[GENERATE_INIT_POSITION_HEIGHT_NUM * GENERATE_INIT_POSITION_WIDTH_NUM+1];

	indexQueue.push_back(0);
	float widthSize = (float)mTerrainWidth / GENERATE_INIT_POSITION_WIDTH_NUM;
	float heightSize = (float)mTerrainHeight / GENERATE_INIT_POSITION_HEIGHT_NUM;

	// 1) 블럭을 나누고 초기값 넣기
	for ( int i = 0; i < GENERATE_INIT_POSITION_WIDTH_NUM; i++)
	{
		for ( int j = 0; j < GENERATE_INIT_POSITION_HEIGHT_NUM; j++)
		{
			mInitPosition.push_back(pbVecf( widthSize/2 + i*widthSize,0, heightSize/2 + j*heightSize));
			positionFlag[i*GENERATE_INIT_POSITION_WIDTH_NUM + j + 1] = false;
		}
	}
	positionFlag[0] = true;

	// 2) 블럭의 중심에서 다른 블럭의 중심에 연결이 되는지 확인
	int wallNum = mWallInfoVector.size();
	while( !indexQueue.empty() )
	{
		int index = indexQueue.back();
		indexQueue.pop_back();

		// 각각 점에 대해 검사
		for( int i = 0; i < GENERATE_INIT_POSITION_HEIGHT_NUM*GENERATE_INIT_POSITION_WIDTH_NUM+1; i++)
		{
			// 이미 확인된 것은 건너 뜀
			if ( positionFlag[i] )
				continue;
			
			// 각각 벽에 대해 검사
			bool isCross = false;
			for ( int j = 0 ; j < wallNum; j++)
			{
				if ( isLineCross(mInitPosition[i], mInitPosition[index], pbVecf(mWallInfoVector[j].startX,0,mWallInfoVector[j].startZ), pbVecf(mWallInfoVector[j].endX, 0, mWallInfoVector[j].endZ)) )
				{
					isCross = true;
					break;
				}
			}

			if ( !isCross )
			{
				indexQueue.push_back(i);
				positionFlag[i] = true;
			}

			
		}
	}

	// 3) 생성 가능 위치만 저장을 한다.
	vector<pbVecf> temp;
	for( int i = 0; i < GENERATE_INIT_POSITION_HEIGHT_NUM*GENERATE_INIT_POSITION_WIDTH_NUM+1; i++)
	{
		if ( positionFlag[i] )
			temp.push_back(mInitPosition[i]);
	}

	mInitPosition = temp;

}


pbTerrain::pbTerrain()
{

}

pbTerrain::~pbTerrain()
{
	delete[] mWallTexture;
}


bool pbTerrain::isPossiblePosition(pbVecf point, vector<pbVecf> objectPoint, float range)
{
	int objectNum = objectPoint.size();
	for ( int i = 0; i < objectNum; i++ )
	{
		if ( pow(point.x - objectPoint[i].x,2) + pow(point.z - objectPoint[i].z,2 ) < pow(range,2) )
			return false;
	}
	return true;
}


bool pbTerrain::isInRange(WallInfo wall, pbVecf point)
{

	float largeX = ( wall.startX > wall.endX ? wall.startX : wall.endX );
	float smallX = ( wall.startX > wall.endX ? wall.endX : wall.startX );

	float largeY = ( wall.startZ > wall.endZ ? wall.startZ : wall.endZ );
	float smallY = ( wall.startZ > wall.endZ ? wall.endZ : wall.startZ );

	if ( (largeX - smallX) == 0 && point.z > smallY && point.z < largeY )
		return true;
	else if ( (largeY - smallY ) == 0 && point.x > smallX && point.x < largeX )
		return true;
	else if ( point.x > smallX && point.x < largeX && point.z > smallY && point.z < largeY)
		return true;
	else
		return false;
}

bool pbTerrain::isInCircle(pbVecf point, pbVecf circleMiddlePoint, float circleRadius)
{
	float x = point.x - circleMiddlePoint.x;
	float z = point.z - circleMiddlePoint.z;

	float powDistance = (float)pow(x,2) + (float)pow(z,2);
	float powRadius = (float)pow(circleRadius,2);

	if ( powDistance  > powRadius )
		return false;
	else
		return true;
}

bool pbTerrain::isSingleWallCollision(pbVecf position1, pbVecf position2, WallInfo wall)
{
	float x1, y1, x2, y2, x3, y3, x4, y4;

	x1 = wall.startX;
	y1 = wall.startZ;
	x2 = wall.endX;
	y2 = wall.endZ;
	x3 = position1.x;
	y3 = position1.z;
	x4 = position2.x;
	y4 = position2.z;

	// ccw 알고리즘
	float res1 = (x1*y2 + x2*y3 + x3*y1) - (x1*y3 + x2*y1 + x3*y2 );
	float res2 = (x1*y2 + x2*y4 + x4*y1) - (x1*y4 + x2*y1 + x4*y2 );

	if ( res1 < 0 && res2 < 0)
		return false;
	else if (res1 > 0 && res2 > 0 )
		return false;
	else
		return true;
}



bool pbTerrain::isLineCross(pbVecf line0StartPoint, pbVecf line0EndPoint, pbVecf line1StartPoint, pbVecf line1EndPoint)
{
	// line0 = (x1,y1) (x2,y2)
	// line1 = (x3,y3) (x4,y4)
	// den = (y4, - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1)
	// if den == 0 parallel -> 교차하지 않음 -> false
	// ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / den
	// ub = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / den
	// if 0 < ua < 1 && 0 < ub < 1 -> 교차함 -> true
	// else (ua == 0 || ua == 1 || ub == 0 || ub == 1) 점으로 교차 -> true
	// else 교차하지 않음 -> false
	float den = (line1EndPoint.z - line1StartPoint.z ) * ( line0EndPoint.x - line0StartPoint.x ) - (line1EndPoint.x - line1StartPoint.x ) * (line0EndPoint.z - line0StartPoint.z);
	if ( den == 0 )
		return false;

	float ua = ( (line1EndPoint.x - line1StartPoint.x ) * ( line0StartPoint.z - line1StartPoint.z ) - (line1EndPoint.z - line1StartPoint.z ) * ( line0StartPoint.x - line1StartPoint.x) ) / den;
	float ub = ( (line0EndPoint.x - line0StartPoint.x ) * ( line0StartPoint.z - line1StartPoint.z ) - (line0EndPoint.z - line0StartPoint.z ) * ( line0StartPoint.x - line1StartPoint.x) ) / den;

	if ( ua < 0 || ua > 1 || ub < 0 || ub > 1)
		return false;
	else
		return true;
}



void clearLastString(char* str, int len)
{

	for(int i = 0; i < len; i++)
	{
		if( str[i] == '\0' || str[i] == '\n' )
		{
			str[i] = '\0';
			break;
		}

	}

}