#include <stdio.h>
#include <iostream>


#define TEST_MODE
#include "pbWorld.h"

using std::to_string;

const int catchRange = 5;

const int pbWorld::PLAYER_ALL_STRING_LEN = 39;
const int pbWorld::PLAYER_STATE_STRING_LEN = 12;
const int pbWorld::GHOST_ALL_STRING_LEN = 37;
const int pbWorld::SINGLE_PLAYER_NUM = 3;

#define PI 3.14159265

void pbWorld::render()
{
	// 초기화
	gluLookAt(0,3,0,0,3,1,0,1,0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	GLfloat ambientLight[] = {0.3,0.3, 0.3, 1.0f};
	float fFogColor[3] = {0.8f, 0.8f, 0.8f};


	// player의 상태에 따른 다른 색
	if ( mPlayerVector[mMyPlayerId].getTypeState() == NORMAL )
	{
		ambientLight[0] = 0.7;
		ambientLight[1] = 0.7;
		ambientLight[2] = 0.7;
		fFogColor[0] = 0.8;
		fFogColor[1] = 0.8;
		fFogColor[2] = 0.8;
	}
	else
	{
		ambientLight[0] = 0.1;
		ambientLight[1] = 0.1;
		ambientLight[2] = 0.1;
		fFogColor[0] = 0.5;
		fFogColor[1] = 0.5;
		fFogColor[2] = 0.5;
	}

	//
	// light
	//
	GLfloat lightColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat lightPos[] = {0, 0, 0, 0.0f};
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	//
	// 플레이어 위치에 따른 지형 위치 이동 및 회전
	//
	glRotatef(mPlayerVector[mMyPlayerId].getVerticalAngle(), 1, 0, 0);
	glRotatef(-mPlayerVector[mMyPlayerId].getHorizonAngle(), 0, 1, 0);
	glTranslatef(-mPlayerVector[mMyPlayerId].getPosition().x,0,-mPlayerVector[mMyPlayerId].getPosition().z);
	
	//
	// fog
	//
	glEnable(GL_FOG);
	glFogfv(GL_FOG_COLOR, fFogColor);
	glFogi(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, 0.02f);
	glHint(GL_FOG_HINT, GL_NICEST);
		
	//
	// 지형 출력
	//
	glEnable(GL_TEXTURE_2D);
	mTerrain.render();
	glDisable(GL_TEXTURE_2D);

	//
	// ghost render
	//
	int ghostNum = mGhostVector.size();
	for(int i = 0; i < ghostNum; i++)
	{
		if(mGhostVector[i].getTargetPlayerId() == mMyPlayerId)
			mGhostVector[i].render(true);
		else
			mGhostVector[i].render(false);

	}

	//
	// player render
	//
	
	int playerNum = mPlayerVector.size();
	
	for(int i = 0; i < playerNum; i++)
	{
		if(mMyPlayerId == i)
			continue;
		else if(mPlayerVector[mMyPlayerId].getTarget() == i)
			mPlayerVector[i].render(true);
		else
			mPlayerVector[i].render(false);

	}
}

void pbWorld::genInitialGhostPosition()
{
	int terrainWidth = mTerrain.getTerrainWidth();
	int terrainHeight = mTerrain.getTerrainHeight();

	int counter = 0;
	while ( counter < 20 )
	{
		int ghostNum = mGhostVector.size();
		float width = (float)(rand() % terrainWidth );
		float height = (float)(rand() % terrainHeight );

		bool inTerritory = false;
		for ( int i = 0; i < ghostNum; i++)
		{
			if ( lineDistanceSquare ( pbVecf(width, 0, height), mGhostVector[i].getPosition() )< mGhostVector[i].getTerritoryRange()*mGhostVector[i].getTerritoryRange() ) 
			{
				inTerritory = true;
				break;
			}
		}

		if ( inTerritory )
			counter++;
		else
		{
			mGhostVector.push_back(pbGhost(pbVecf(width, 0, height)));
			 
			counter = 0;
		}

	}

	
}

void pbWorld::addHostPlayer()
{
	pbVecf position = mTerrain.getAblePosition(getObjectVector(), pbPlayer::getNewPlayerDistantRange());
	mPlayerVector.push_back(pbPlayer(position, -1));
	mMyPlayerId = 0;
}

void pbWorld::addSinglePlayer()
{
	for ( int i = 0; i < SINGLE_PLAYER_NUM; i++)
	{
		pbVecf position = mTerrain.getAblePosition(getObjectVector(), pbPlayer::getNewPlayerDistantRange());

		pbPlayer singlePlayer;
		singlePlayer.setPosition(position);

		// setting target of new player and new player's target
		int targetPlayerId = getRandomLivePlayerId();	
		if ( targetPlayerId == -1 )
		{
			singlePlayer.setTarget(-1);
		}
		else
		{
			if ( mPlayerVector[targetPlayerId].getTarget() == -1 )
			{
				singlePlayer.setTarget(targetPlayerId);
				mPlayerVector[targetPlayerId].setTarget(i+1);
			}
			else
			{
				singlePlayer.setTarget(mPlayerVector[targetPlayerId].getTarget());
				mPlayerVector[targetPlayerId].setTarget(i+1);
			}
		}

		mPlayerVector.push_back(singlePlayer);
	}
}


void pbWorld::addClientPlayer(int newPlayerId)
{
	if ( newPlayerId == -1 )
		return;
	
	// setting new player's position
	pbVecf position = mTerrain.getAblePosition(getObjectVector(), pbPlayer::getNewPlayerDistantRange());

	pbPlayer newPlayer;
	newPlayer.setPosition(position);


	// setting target of new player and new player's target
	int targetPlayerId = getRandomLivePlayerId();	
	if ( targetPlayerId == -1 )
	{
		newPlayer.setTarget(-1);
	}
	else
	{
		if ( mPlayerVector[targetPlayerId].getTarget() == -1 )
		{
			newPlayer.setTarget(targetPlayerId);
			mPlayerVector[targetPlayerId].setTarget(newPlayerId);
		}
		else
		{
			newPlayer.setTarget(mPlayerVector[targetPlayerId].getTarget());
			mPlayerVector[targetPlayerId].setTarget(newPlayerId);
		}
	}

	// add (modify) player in player vector & add (modify) pre recv buffer
	if ( newPlayerId < mPlayerVector.size() )
	{
		mPreRecvBuffer[newPlayerId].clear();
		mPlayerVector[newPlayerId] = newPlayer;
	}
	else
	{
		mPreRecvBuffer.push_back(string());
		mPlayerVector.push_back(newPlayer);
	}
}

void pbWorld::disconnectClientPlayer(int disconnectPlayerId)
{
	if ( disconnectPlayerId == -1 )
		return;
	
	printf("======================disconnect\n");
	if ( mPlayerVector[disconnectPlayerId].getTypeState() == NORMAL)
	{
		int playerNum = mPlayerVector.size();
		for ( int i = 0; i < playerNum; i++ )
		{
			if ( mPlayerVector[i].getTarget() == disconnectPlayerId )
			{
				mPlayerVector[i].setTarget(mPlayerVector[disconnectPlayerId].getTarget());
				break;
			}
		}

	}

	mPlayerVector[disconnectPlayerId].setTypeState(OUT_OF_GAME);

}



void pbWorld::genInitialPlayerPosition()
{
	mTerrain.genInitPosition();
}

int pbWorld::loadMap()
{
	mTerrain.loadMapFromFile();
	mTerrain.makeDisplayList();

	return 0;
}


vector<pbVecf> pbWorld::getObjectVector()
{
	vector<pbVecf> objectVector;
	int playerVectorNum = mPlayerVector.size();
	int ghostVectorNum = mGhostVector.size();

	for( int i = 0; i < playerVectorNum; i++)
	{
		objectVector.push_back(mPlayerVector[i].getPosition());
	}
	for( int i = 0; i < ghostVectorNum; i++)
	{
		objectVector.push_back(mGhostVector[i].getPosition());
	}

	return objectVector;
}

int pbWorld::getRandomLivePlayerId() 
{
	vector<int> livePlayerIdVector;

	int playerNum = mPlayerVector.size();
	for ( int i = 0; i < playerNum; i++ )
	{
		if ( mPlayerVector[i].getTypeState() == NORMAL )
			livePlayerIdVector.push_back(i);
	}

	if ( livePlayerIdVector.size() == 0)
		return -1;

	return livePlayerIdVector[rand()%livePlayerIdVector.size()];
}

void pbWorld::advanceModels()
{
	pbPlayer::advanceModel();
	pbGhost::advanceModel();
}

string pbWorld::getPositionInfo(bool isHost)
{
	string positionString;
	char temp[64];
	int playerNum = mPlayerVector.size();

	// <player, all, id, position x, position z, horizon angle, target, state(normal,dead) >
	// <p,a,00,0000.000,0000.000,0000.000,00,n,> 2+2+3+9+9+9+3+2 = 39
	// <player, state, id, target, state>
	// <p,s,00,00,g,> 2+2+3+3+2 = 12
	// <ghost, all, id, position x, position z, horizon angle, target>
	// <g,a,00,0000.000,0000.000,0000.000,00,> 2+2+3+9+9+9+3 = 37

	if ( isHost )
	{
		// host sends all
		for( int i = 0; i < playerNum; i++)
		{
			// 2 + 3 + 9+9+9+9+3+2
			positionString += "p,";	// player
			positionString += "a,";	// all
			sprintf_s(temp,"%02d,",i);
			positionString += temp;	// id
			sprintf_s(temp,"%08.3f,",mPlayerVector[i].getPosition().x);
			positionString += temp;	// position x
			sprintf_s(temp,"%08.3f,",mPlayerVector[i].getPosition().z);
			positionString += temp;	// position z
			sprintf_s(temp,"%08.3f,",mPlayerVector[i].getHorizonAngle());
			positionString += temp;	// horizion angle
			sprintf_s(temp,"%02d,",mPlayerVector[i].getTarget());
			positionString += temp;	// target id

			switch ( mPlayerVector[i].getTypeState() )	// state
			{
			case NORMAL:
				positionString += "n,";
				break;
			case DEAD:
				positionString += "d,";
				break;
			default:	// OUT_OF_GAME
				positionString += "o,";
			}

		}
		int ghostNum = mGhostVector.size();
		for ( int i = 0; i < ghostNum; i++)
		{
			// 2+3+9+9+9+3

			positionString += "g,";	// ghost
			positionString += "a,";	// all
			sprintf_s(temp,"%02d,",i);
			positionString += temp;	// id
			sprintf_s(temp,"%08.3f,",mGhostVector[i].getPosition().x);
			positionString += temp;	// position x
			sprintf_s(temp,"%08.3f,",mGhostVector[i].getPosition().z);
			positionString += temp;	// position y
			sprintf_s(temp,"%08.3f,",mGhostVector[i].getViewAngle());
			positionString += temp;	// horizon angle
			sprintf_s(temp,"%02d,",mGhostVector[i].getTargetPlayerId());
			positionString += temp;	//target
		
		}
	}
	else
	{
		// <player, all, id, position x, position z, horizon angle, target, state(normal,dead) >
		// <p,a,00,0000.000,0000.000,0000.000,00,n,> 2+2+3+9+9+9+3+2 = 39
		// <player, state, id, target, state>
		// <p,s,00,00,g,> 2+2+3+3+2 = 12

		// if client catch other player
		int clientNum = mPlayerVector.size();
		for ( int i = 0; i < clientNum; i++)
		{
			if ( i == mMyPlayerId )
			{
				// client sends only my info
				positionString += "p,";	// player
				positionString += "a,";	// state
				sprintf_s(temp,"%02d,",mMyPlayerId);
				positionString += temp;	// id
				sprintf_s(temp,"%08.3f,",mPlayerVector[mMyPlayerId].getPosition().x);
				positionString += temp;	// position x
				sprintf_s(temp,"%08.3f,",mPlayerVector[mMyPlayerId].getPosition().z);
				positionString += temp;	// position z
				sprintf_s(temp,"%08.3f,",mPlayerVector[mMyPlayerId].getHorizonAngle());
				positionString += temp;	// horizion angle
				sprintf_s(temp,"%02d,",mPlayerVector[mMyPlayerId].getTarget());
				positionString += temp;	// target id
				switch ( mPlayerVector[mMyPlayerId].getTypeState() )	// state
				{
				case NORMAL:
					positionString += "n,";
					break;
				case DEAD:
					positionString += "d,";
					break;
				default:	// OUT_OF_GAME
					positionString += "o,";
				}

			}

			if ( mPlayerVector[i].getTypeChangeFlag() )
			{
				positionString += "p,";	// player
				positionString += "s,";	// state
				sprintf_s(temp,"%02d,", i);	
				positionString += temp;	// id
				sprintf_s(temp,"%02d,",mPlayerVector[i].getTarget());
				positionString += temp;	// target
				switch ( mPlayerVector[i].getTypeState() )	// state
				{
				case NORMAL:
					positionString += "n,";
					break;
				case DEAD:
					positionString += "d,";
					break;
				default:	// OUT_OF_GAME
					positionString += "o,";
				}
				
				mPlayerVector[i].setTypeChangeFlag(false);
			}
		}

	}
	
	return positionString;	
}
void pbWorld::updatePlayersPosition(vector<string> strVector)
{
	
	int recvBufferNum = strVector.size();
	
	if ( recvBufferNum == 0 )
		return;

	// 버퍼 수가 안맞을때
	while ( recvBufferNum > mPreRecvBuffer.size() )
	{
		mPreRecvBuffer.push_back(string());
	}


	for ( int i = 0; i < recvBufferNum; i++)
	{
		if ( strVector[i].size() == 0)
			continue;

		mPreRecvBuffer[i] += strVector[i];
		int index = 0;
		int strLen = mPreRecvBuffer[i].size();

		while ( index < strLen )
		{
			if ( mPreRecvBuffer[i][index] == 'p' )
			{
				// <player, all, id, position x, position z, horizon angle, target, state(normal,dead) >
				// <p,a,00,0000.000,0000.000,0000.000,00,n,> 2+2+3+9+9+9+3+2 = 39
				// <player, state, id, target, state>
				// <p,s,00,00,g,> 2+2+3+3+2 = 12

				// 2번째 정보 존재 여부
				if ( strLen < index + 3 )
					break;

				int playerStringLen;
				if ( mPreRecvBuffer[i][index+2] == 'a' )
					playerStringLen = PLAYER_ALL_STRING_LEN;
				else
					playerStringLen = PLAYER_STATE_STRING_LEN;

				// 해당 정보가 다 있는지 체크
				if ( strLen < index + playerStringLen )
					break;

				// 실제 정보 입력
				if ( mPreRecvBuffer[i][index+2] == 'a' )
				{
					

					int id = stoi(mPreRecvBuffer[i].substr(index+4,2));

					// if new player, add player vector ( use in clinet )
					while ( id + 1 > mPlayerVector.size() )
					{
						mPlayerVector.push_back(pbPlayer());
					}

					
					if ( id == mMyPlayerId )
					{
						// 클라가 스스로의 정보를 받는경우
						// 초기값 무조건 변경, 그외는 상태와 타겟만 변경함
						if ( !mInitPosition )
						{
							// 처음 값
							mPlayerVector[id].setPosition(pbVecf(stof(mPreRecvBuffer[i].substr(index+7,8)),0,stof(mPreRecvBuffer[i].substr(index+16,8))));
							mPlayerVector[id].setLookAngle(0,stof(mPreRecvBuffer[i].substr(index+25,8)));
							mPlayerVector[id].setTarget(stod(mPreRecvBuffer[i].substr(index+34,2)));
							switch ( mPreRecvBuffer[i][index+37] )
							{
							case 'd':
								mPlayerVector[id].setTypeState(DEAD);
								break;
							case 'n':
								mPlayerVector[id].setTypeState(NORMAL);
								break;
							default:
								mPlayerVector[id].setTypeState(OUT_OF_GAME);
							}

							mInitPosition = true;
						}
						else
						{
							mPlayerVector[id].setTarget(stod(mPreRecvBuffer[i].substr(index+34,2)));
							if ( mPreRecvBuffer[i][index+37] == 'd' )
							{
								mPlayerVector[mMyPlayerId].setTypeState(DEAD);
							}
							else
							{
								mPlayerVector[mMyPlayerId].setTypeState(NORMAL);
							}
						}
					}
					else
					{
						// 호스트가 다른 클라 정보 받는경우
						// 클라이언트가 호스트로부터 다른 클라 정보 받는 경우
						// 무조건 변경
						if ( mPlayerVector[id].getTypeState() == OUT_OF_GAME)
						{
							index += playerStringLen;
							continue;
						}
						mPlayerVector[id].setPosition(pbVecf(stof(mPreRecvBuffer[i].substr(index+7,8)),0,stof(mPreRecvBuffer[i].substr(index+16,8))));
						mPlayerVector[id].setLookAngle(0,stof(mPreRecvBuffer[i].substr(index+25,8)));


						if ( !mPlayerVector[id].getTypeChangeFlag() )
						{
							mPlayerVector[id].setTarget(stod(mPreRecvBuffer[i].substr(index+34,2)));
							switch ( mPreRecvBuffer[i][index+37] )
							{
							case 'd':
								mPlayerVector[id].setTypeState(DEAD);
								break;
							case 'n':
								mPlayerVector[id].setTypeState(NORMAL);
								break;
							default:
								mPlayerVector[id].setTypeState(OUT_OF_GAME);
							}
						}
					}

					
				}
				else if ( mPreRecvBuffer[i][index+2] == 's' )
				{
					// <player, state, id, target, state>
					// <p,s,00,00,g,> 2+2+3+3+2 = 12

					// only host can recv this message
					// 무조건 변경


					int id = stoi(mPreRecvBuffer[i].substr(index+4,2));

					if ( mPlayerVector[id].getTypeState() == OUT_OF_GAME)
					{
						index += playerStringLen;
						continue;
					}
					mPlayerVector[id].setTarget(stod(mPreRecvBuffer[i].substr(index+7,2)));
					switch ( mPreRecvBuffer[i][index+10] )
					{
					case 'd':
						mPlayerVector[id].setTypeState(DEAD);
						break;
					case 'n':
						mPlayerVector[id].setTypeState(NORMAL);
						break;
					default:
						mPlayerVector[id].setTypeState(OUT_OF_GAME);
					}
				}

				index += playerStringLen;
			}
			else if ( mPreRecvBuffer[i][index] == 'g')
			{
				// <ghost, all, id, position x, position z, horizon angle, target>
				// <g,a,00,0000.000,0000.000,0000.000,00,> 2+2+3+9+9+9+3 = 37
				
				int ghostStringLen;
				ghostStringLen = GHOST_ALL_STRING_LEN;
				if ( strLen < index + ghostStringLen )
					break;


				int id = stoi(mPreRecvBuffer[i].substr(index+4,2));

				while ( id + 1 > mGhostVector.size() )
				{
					mGhostVector.push_back(pbGhost());
				}



				mGhostVector[id].setPosition(pbVecf(stof(mPreRecvBuffer[i].substr(index+7,8)),0,stof(mPreRecvBuffer[i].substr(index+16,8))));
				mGhostVector[id].setLookAngle(stof(mPreRecvBuffer[i].substr(index+25,8)));
				mGhostVector[id].setTargetPlayerId(stod(mPreRecvBuffer[i].substr(index+34,2)));

				index += ghostStringLen;
			}
			else
			{
				if ( mPreRecvBuffer[i][index] == 'p' || mPreRecvBuffer[i][index] == 'g' )
					continue;
				else
					index++;
			}
		}
		mPreRecvBuffer[i] = mPreRecvBuffer[i].substr(index);
	}
	
}


pbWorld::pbWorld()
{
	mInitPosition = false;

}
pbWorld::~pbWorld()
{
	
}
void pbWorld::cameraMove(float horizonMove, float verticalMove)
{
	mPlayerVector[mMyPlayerId].moveLookAt(horizonMove, verticalMove);
}


void pbWorld::playerMove(int frontDirection, int rightDirection)
{
	pbVecf nextPosition = mPlayerVector[mMyPlayerId].getNextPosition(frontDirection, rightDirection);

	if ( mTerrain.isWallCollision(nextPosition, mPlayerVector[mMyPlayerId].getPosition()) )
		return;
	else
	{
		mPlayerVector[mMyPlayerId].setPosition(nextPosition);
	}
	
}



void pbWorld::tryCatchPlayer()
{
	if ( mPlayerVector[mMyPlayerId].getTarget() == -1 )
		return;
	
	if( mPlayerVector[mMyPlayerId].getTypeState() == NORMAL)
	{
		pbVecf myPos = mPlayerVector[mMyPlayerId].getPosition();
		int targetId = mPlayerVector[mMyPlayerId].getTarget();
		pbVecf targetPos = mPlayerVector[targetId].getPosition();
		if( sqrt( pow(myPos.x - targetPos.x, 2) + pow(myPos.z - targetPos.z, 2)) < catchRange )
		{
			// catch함
			mPlayerVector[targetId].setTypeState(DEAD);
			if ( mPlayerVector[targetId].getTarget() == mMyPlayerId )
			{
				// 플레이어가 오직 2명뿐인 경우
				mPlayerVector[mMyPlayerId].setTarget(-1);
			}
			else
				mPlayerVector[mMyPlayerId].setTarget(mPlayerVector[targetId].getTarget());
		
			
			mPlayerVector[targetId].setTarget(-1);
			mPlayerVector[targetId].setTypeChangeFlag(true);
			
		}
	}
	else if (mPlayerVector[mMyPlayerId].getTypeState() == DEAD )
	{
		// 죽은 사람이 살아있는 사람을 잡을때 ( 미구현 )
		/*
		pbVecf myPos = mPlayerVector[myPlayerId].getPosition();
		pbVecf targetPos;
		int playerNum = mPlayerVector.size();
		for(int i = 0; i < playerNum; i++)
		{
			if(mPlayerVector[i].getTypeState() == NORMAL)
			{
				targetPos = mPlayerVector[i].getPosition();
				if( sqrt( pow(myPos.x - targetPos.x, 2) + pow(myPos.z - targetPos.z, 2)) < catchRange )
				{
					mPlayerVector[i].setTypeState(DEAD);
					mPlayerVector[myPlayerId].setTarget(mPlayerVector[i].getTarget());
					mPlayerVector[i].setTarget(-1);
					for(int j = 0; j < playerNum; j++)
					{
						if(mPlayerVector[j].getTarget() == i)
							mPlayerVector[j].setTarget(myPlayerId);
					}
				}
			}
	
		}
		*/
		
	}
}

void pbWorld::ghostMove()
{
	int ghostNum = mGhostVector.size();
	for( int i = 0; i < ghostNum; i++)
	{
		mGhostVector[i].findTargetPlayer(mPlayerVector);
		mGhostVector[i].moveNextPosition();
	}
}

void pbWorld::loadModel()
{

	pbPlayer::loadModel();

	pbGhost::loadModel();
}

void pbWorld::setMyPlayerId(int id)
{
	mMyPlayerId = id;
}

