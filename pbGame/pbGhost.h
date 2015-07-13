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
	void findTargetPlayer(vector<pbPlayer> &playerVector);	// 플레이어들과 거리를 비교해서 목표를 설정

	// ghost 모델 처리
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

	// 시작, 끝 위치 정보
	pbVecf mStartVec;
	pbVecf mEndVec;
	pbVecf mAimPoint;

	// 위치, 보는 각도
	pbVecf mPosition;
	float  mHorizonLookAngle;
	
	// 기타 정보
	bool      mMoveDirectionFlag;	// true->center, false->aim point
	int       mTargetPlayerId;
	pbPlayer* mTargetPlayer;

	// 상수 값
	static const float GHOST_MOVE_SPEED;
	static const float GHOST_SET_TARGET_RANGE;
	static const float GHOST_CATCH_TARGET_RANGE;
	static const int GHOST_MAX_TERRITORY_RANGE;
	static const int GHOST_MIN_TERRITORY_RANGE;

	// 유령 모델
	static MD2Model* mModel;
	
};