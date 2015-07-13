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
	
	
	// �÷��̾� �� ó��
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
	// ��ġ, ���� ����
	pbVecf mPosition;
	float  mVerticalLookAngle;
	float  mHorizionLookAngle;

	// ���� ����
	PlayerTypeState mTypeState;			// NORMAL, GHOST, OUT_OF_GAME
	bool            mTypeChangeFlag;	// type state�� ���� �Ǿ����� ( client�� ���� �������� host�� ���� �ϱ� ���� ���̴� flag ) 
	int             mTargetPlayerId;
	
	
	// �����
	static const float PLAYER_DEAD_SPEED;
	static const float PLAYER_NORMAL_SPEED;
	static const float MODEL_ADVANCE_SPEED;
	static const float NEW_PLAYER_OBJECT_DISTANT_RANGE;
	
	// �÷��̾� ��
	static MD2Model* mModel;
};