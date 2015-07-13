#pragma once

#include <vector>
using std::vector;

#include "pbDef.h"
#include "pbPlayer.h"
#include "md2model.h"

class pbGhost {
public:
	void render(bool isChasing);

	void moveNextPosition();
	void findTargetPlayer(vector<pbPlayer> &playerVector);	// �÷��̾��� �Ÿ��� ���ؼ� ��ǥ�� ����

	// ghost �� ó��
	static void loadModel();
	static void advanceModel();

	// setter, getter
	void setTarget(pbPlayer* player);
	void setPosition(pbVecf position);
	void setTargetPlayerId(int id);
	void setLookAngle(float horizonLookAngle);
	
	pbVecf getPosition();
	int   getTargetPlayerId();
	float getViewAngle();
	float getTerritoryRange();

public:
	pbGhost();
	pbGhost(pbVecf centerPoint);

private:
	void moveTo(pbVecf des);
	void setupAimPoint();

private:
	
	pbVecf mCenterPoint;
	float  mTerritoryRange;

	// ����, �� ��ġ ����
	pbVecf mStartVec;
	pbVecf mEndVec;
	pbVecf mAimPoint;

	// ��ġ, ���� ����
	pbVecf mPosition;
	float  mHorizonLookAngle;
	
	// ��Ÿ ����
	bool      mMoveDirectionFlag;	// true->center, false->aim point
	int       mTargetPlayerId;
	pbPlayer* mTargetPlayer;

	// ��� ��
	static const float GHOST_MOVE_SPEED;
	static const float GHOST_SET_TARGET_RANGE;
	static const float GHOST_CATCH_TARGET_RANGE;
	static const int GHOST_MAX_TERRITORY_RANGE;
	static const int GHOST_MIN_TERRITORY_RANGE;

	// ���� ��
	static MD2Model* mModel;
	
};