#pragma once

class pbKeyInput {
public:
	void normalKeyboardPress(unsigned char key);
	void normalKeyboardUp(unsigned char key);
	void specialKeyboardPress(int key);
	void specialKeyboardUp(int key);

	bool isKeyPressed();

public:
	pbKeyInput();

private:

	bool normalKey[256];
	bool specialKey[32];

	int keyboardPressCount;



};