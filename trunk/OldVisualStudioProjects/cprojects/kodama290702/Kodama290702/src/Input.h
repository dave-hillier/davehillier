// Input.h: interface for the Input class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __INPUT_H__
#define __INPUT_H__

#include "Keycodes.h"


class Input  
{
public:
	Input();
	virtual ~Input();

	void KeyUp (int key);
	void KeyDown (int key);

	char *KeyString (int code);
	int Code (char *str);

	inline bool State (int key) { return keystate[key]; };
private:
	bool keystate[NUM_KEYS];

};

extern Input *input;

#endif