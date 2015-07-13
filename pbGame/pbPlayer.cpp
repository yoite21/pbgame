#include <math.h>

#include "pbPlayer.h"

#define PI 3.14159265

const float pbPlayer::PLAYER_DEAD_SPEED = 0.7f;
const float pbPlayer::PLAYER_NORMAL_SPEED = 1.0f;
const float pbPlayer::MODEL_ADVANCE_SPEED = 0.025f;
const float pbPlayer::NEW_PLAYER_OBJECT_DISTANT_RANGE = 50.0f;
MD2Model* pbPlayer::mModel;


void pbPlayer::render(bool isTarget)
{
	if ( mTypeState == OUT_OF_GAME )
		return;

	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();

	glTranslatef(mPosition.x, 0, mPosition.z);
	
	if(isTarget)
		glColor3f(1,0,0);
	else
		glColor3f(1,1,1);

	if(mTypeState == DEAD)
		glColor3f(0.3, 0.3, 0.3);

	
	if(mModel != NULL)
	{
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);

		glTranslatef(0.0f, 4.0f, 0.0f);
		glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(-mHorizionLookAngle, 1.0f, 0.0f, 0.0f);
		glScalef(0.2f, 0.2f, 0.2f);

		mModel->draw();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}

	glPopMatrix();
	glPopAttrib();
}

int pbPlayer::getTarget()
{
	return mTargetPlayerId;
}

pbVecf pbPlayer::getNextPosition(int frontDirection, int rightDirection)
{
	float speed;
	if( mTypeState == DEAD )
		speed = PLAYER_DEAD_SPEED;
	else
		speed = PLAYER_NORMAL_SPEED;

	float angle;
	if( frontDirection > 0 && rightDirection == 0)
		angle = 0;
	else if ( frontDirection > 0 && rightDirection < 0)
		angle = 45;
	else if ( frontDirection == 0 && rightDirection < 0 )
		angle = 90;
	else if ( frontDirection < 0 && rightDirection < 0 )
		angle = 135;
	else if ( frontDirection < 0 && rightDirection == 0 )
		angle = 180;
	else if ( frontDirection < 0 && rightDirection > 0 )
		angle = 225;
	else if ( frontDirection == 0 && rightDirection > 0 )
		angle = 270;
	else if ( frontDirection > 0 && rightDirection > 0 )
		angle = 315;
	else
		return mPosition;

	return pbVecf(mPosition.x + speed * ((float)sin((angle + mHorizionLookAngle) * PI / 180)), 0,
		mPosition.z + speed * ((float)cos((angle + mHorizionLookAngle) * PI / 180)));
}


void pbPlayer::setPosition(pbVecf position)
{
	mPosition = position;
}

pbVecf pbPlayer::getPosition()
{
	return mPosition;
}


void pbPlayer::moveLookAt(float horizonMove, float verticalMove)
{
	mVerticalLookAngle -= verticalMove;
	mHorizionLookAngle -= horizonMove;

	if(mVerticalLookAngle > 80)
		mVerticalLookAngle = 80;
	if(mVerticalLookAngle < -80)
		mVerticalLookAngle = -80;

	if(mHorizionLookAngle > 360)
		mHorizionLookAngle -= 360;
	if(mHorizionLookAngle < -360)
		mHorizionLookAngle += 360;
}

void pbPlayer::setLookAngle(float verticalLookAngle, float horizonLookAngle)
{
	mVerticalLookAngle = verticalLookAngle;
	mHorizionLookAngle = horizonLookAngle;
}



void pbPlayer::setTypeState(PlayerTypeState state)
{
	mTypeState = state;
}

pbPlayer::pbPlayer()
{ 
	mTypeState = NORMAL;
	mTypeChangeFlag = false;

	mVerticalLookAngle = 0;
	mHorizionLookAngle = 0;

	mTargetPlayerId = -1;

}

pbPlayer::pbPlayer(pbVecf position, int target)
{
	mTypeState = NORMAL;
	mTypeChangeFlag = false;

	mVerticalLookAngle = 0;
	mHorizionLookAngle = 0;
	mPosition = position;
	mTargetPlayerId = target;
}

pbPlayer::~pbPlayer()
{
	
}

float pbPlayer::getNewPlayerDistantRange()
{
	return NEW_PLAYER_OBJECT_DISTANT_RANGE;
}

PlayerTypeState pbPlayer::getTypeState()
{
	return mTypeState;
}


void pbPlayer::setTarget(int id)
{
	mTargetPlayerId = id;
}

void pbPlayer::loadModel()
{
	mModel = MD2Model::load("resource/tallguy.md2");
		if (mModel != NULL) {
		mModel->setAnimation("run");
	}
}

void pbPlayer::advanceModel()
{
	if(mModel != NULL)
		mModel->advance(MODEL_ADVANCE_SPEED);
}

void pbPlayer::setTypeChangeFlag(bool flag)
{
	mTypeChangeFlag = flag;
}

bool pbPlayer::getTypeChangeFlag()
{
	return mTypeChangeFlag;
}


float pbPlayer::getVerticalAngle()
{
	return mVerticalLookAngle;
}

float pbPlayer::getHorizonAngle()
{
	return mHorizionLookAngle;
}