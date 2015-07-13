#include "pbKeyInput.h"


void pbKeyInput::normalKeyboardPress(unsigned char key)
{
	keyboardPressCount++;
	normalKey[(unsigned int)key] = true;

}

void pbKeyInput::normalKeyboardUp(unsigned char key)
{
	keyboardPressCount--;
	normalKey[(unsigned int)key] = false;
}

void pbKeyInput::specialKeyboardPress(int key)
{

}

bool pbKeyInput::isKeyPressed()
{
	if(keyboardPressCount)
		return true;
	else
		return false;
}



pbKeyInput::pbKeyInput()
{
	for(int i = 0; i < 256; i++)
	{
		normalKey[i] = false;
	}

	for(int i = 0; i < 32; i++)
	{
		specialKey[i] = false;
	}
	
	
	keyboardPressCount = 0;
}
