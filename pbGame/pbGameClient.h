#pragma once

#include "pbNetwork.h"
#include "pbWorld.h"

class pbGameClient {
public:

	void render();
	int init();
	void move();
	void updateNetwork();

	void normalKeyboardPress(unsigned char key);
	void normalKeyboardUp(unsigned char key);
	void mouseMove(float x, float y);

public:
	pbGameClient();
private:

	pbWorld   world;
	pbNetwork network;

private:
	bool w_key;
	bool a_key;
	bool s_key;
	bool d_key;
	bool space_key;

	int moveKeyFlag;
};