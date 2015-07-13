#pragma once

#include "pbDef.h"
#include "md2model.h"

#include <vector>
using std::vector;

enum PlayerTypeState {NORMAL, DEAD, OUT_OF_GAME};

class pbPlayer{
public:
	void render(bool isTarget);

	pbVecf getNextPosition(int frontDirection, int rightDirection);
	void moveLookAt(float horizonMove, float verticalMove);
	
	
	// 플레이어 모델 처리
	static void loadModel();
	static void advanceModel();
	static float getNewPlayerDistantRange();
	
	// setter, getter
	void setPosition(pbVecf position);
	void setLookAngle(float verticalLookAngle, float horizonLookAngle);
	void setTarget(int id);
	void setTypeState(PlayerTypeState state);
	void setTypeChangeFlag(bool flag);
	
	pbVecf          getPosition();
	float           getVerticalAngle();
	float           getHorizonAngle();
	int             getTarget();
	PlayerTypeState getTypeState();
	bool            getTypeChangeFlag();
	
public:
	pbPlayer();
	pbPlayer(pbVecf position, int target);
	~pbPlayer();
	
private:
	// 위치, 보는 각도
	pbVecf mPosition;
	float  mVerticalLookAngle;
	float  mHorizionLookAngle;

	// 상태 정보
	PlayerTypeState mTypeState;			// NORMAL, GHOST, OUT_OF_GAME
	bool            mTypeChangeFlag;	// type state가 변경 되었을때 ( client가 변경 시켰을때 host로 전달 하기 위해 쓰이는 flag ) 
	int             mTargetPlayerId;
	
	
	// 상수값
	static const float PLAYER_DEAD_SPEED;
	static const float PLAYER_NORMAL_SPEED;
	static const float MODEL_ADVANCE_SPEED;
	static const float NEW_PLAYER_OBJECT_DISTANT_RANGE;
	
	// 플레이어 모델
	static MD2Model* mModel;
};