
#include <math.h>

#include "pbGhost.h"

#include "glut.h"


#define PI 3.14159265

MD2Model* pbGhost::mModel;

const float pbGhost::GHOST_MOVE_SPEED=0.5;
const float pbGhost::GHOST_SET_TARGET_RANGE = 30.0f;
const float pbGhost::GHOST_CATCH_TARGET_RANGE = 3.0f;
const int pbGhost::GHOST_MAX_TERRITORY_RANGE = 100.0f;
const int pbGhost::GHOST_MIN_TERRITORY_RANGE = 50.0f;

void pbGhost::render(bool isChasing)
{
	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();
	
	if(isChasing)
		glColor3f(0.7f,0.0f,0.0f);
	else
		glColor3f(0.0f,0.0f,1.0f);

	glTranslatef(mPosition.x, 0, mPosition.z);
	glRotatef(270-mHorizonLookAngle, 0.0f, 1.0f, 0.0f);

	if(mModel != NULL)
	{
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);

		glTranslatef(0.0f, 2.0f, 0.0f);
		glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
		glScalef(0.2f, 0.2f, 0.2f);

		mModel->draw();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}

	glPopMatrix();
	glPopAttrib();

	
}


void pbGhost::moveNextPosition()
{
	if(mTargetPlayer != NULL)
	{
		moveTo(mTargetPlayer->getPosition());
	}
	else
	{
		if(mMoveDirectionFlag)
			moveTo(mCenterPoint);	// center
		else
			moveTo(mAimPoint);	// aim point
	}
}


void pbGhost::setPosition(pbVecf position)
{
	mPosition = position;
}

pbVecf pbGhost::getPosition()
{
	return mPosition;
}

void pbGhost::setTargetPlayerId(int id)
{
	mTargetPlayerId = id;
}

int pbGhost::getTargetPlayerId()
{
	return mTargetPlayerId;
}

void pbGhost::setLookAngle(float horizonLookAngle)
{
	mHorizonLookAngle = horizonLookAngle;
}

float pbGhost::getViewAngle()
{
	return mHorizonLookAngle;
}


void pbGhost::setupAimPoint()
{
	int angle = rand() % 360;
	mAimPoint = pbVecf( mCenterPoint.x + mTerritoryRange * cos(angle * PI / 180 ) , 0, mCenterPoint.z + mTerritoryRange * sin(angle * PI / 180));
}



void pbGhost::moveTo(pbVecf des)
{
	

	pbVecf unitVecf = pbVecf(des.x - mPosition.x, 0, des.z - mPosition.z).convertToUnit();
	mPosition.x += unitVecf.x * GHOST_MOVE_SPEED;
	mPosition.z += unitVecf.z * GHOST_MOVE_SPEED;

	if ( unitVecf.z > 0 )
		mHorizonLookAngle = acos(unitVecf.x) * 180.0 / PI;
	else if (unitVecf.z == 0 && unitVecf.x > 0 )
		mHorizonLookAngle = acos(unitVecf.x) * 180.0 / PI;
	else
		mHorizonLookAngle = - (acos(unitVecf.x) * 180.0 / PI);
	

	if( sqrt( lineDistanceSquare( des, mPosition ) ) < 1) 
	{
		if ( mMoveDirectionFlag )
		{
			setupAimPoint();
			mMoveDirectionFlag = false;
		}
		else
		{
			mMoveDirectionFlag = true;
		}
	}
	
}

pbGhost::pbGhost()
{
	mHorizonLookAngle = 0;
	mTargetPlayerId = -1;
}

pbGhost::pbGhost(pbVecf centerPoint)
{
	mCenterPoint = centerPoint;
	mPosition = centerPoint;
	mAimPoint = pbVecf(-1,-1,-1);
	mHorizonLookAngle = 0;
	mTargetPlayerId = -1;

	mTerritoryRange = (float)(rand() % (GHOST_MAX_TERRITORY_RANGE - GHOST_MIN_TERRITORY_RANGE) + GHOST_MIN_TERRITORY_RANGE);
	setupAimPoint();
}

void pbGhost::setTarget(pbPlayer *player)
{
	mTargetPlayer = player;
}

float pbGhost::getTerritoryRange()
{
	return mTerritoryRange;
}


void pbGhost::findTargetPlayer(vector<pbPlayer> &playerVector)
{
	float range;
	float minRange=FLT_MAX;
	int minId=-1;

	int playerNum = playerVector.size();
	for(int i = 0; i < playerNum; i++)
	{
		if(playerVector[i].getTypeState() != NORMAL)
			continue;
		range = sqrt ( lineDistanceSquare( playerVector[i].getPosition(), mPosition ) );
		
		if(range < GHOST_SET_TARGET_RANGE)
		{
			if ( range < minRange )
			{
				minRange = range;
				minId = i;
			}

			if(range < GHOST_CATCH_TARGET_RANGE)
			{
				playerVector[i].setTypeState(DEAD);
				playerVector[i].setTypeChangeFlag(true);
				for(int j = 0; j < playerNum; j++)
				{
					if( playerVector[j].getTarget() == i)
					{
						if ( playerVector[i].getTarget() == j )
							playerVector[j].setTarget(-1);
						else
							playerVector[j].setTarget(playerVector[i].getTarget());
						
						playerVector[i].setTarget(-1);
						break;
					}
				}
			}
		}
	}
	if ( minId != -1 )
	{
		mTargetPlayer = &playerVector[minId];
		mTargetPlayerId = minId;
	}
	else
	{
		mTargetPlayer = NULL;
		mTargetPlayerId = -1;
	}
}


void pbGhost::loadModel()
{
	mModel = MD2Model::load("resource/blockybalboa.md2");
		if (mModel != NULL) {
		mModel->setAnimation("run");
	}
}

void pbGhost::advanceModel()
{
	if(mModel != NULL)
		mModel->advance(0.025f);
}