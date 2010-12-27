// Input.cpp: implementation of the Input class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Input.h"
#include "Binds.h"
#include "Console.h"
#include "ConsoleEdit.h"

KeyCode keynames[] = {

	{ KEY_UNKNOWN, "UNKNOWN" }, 
	{ KEY_FIRST, "FIRST" }, 
	{ KEY_BACKSPACE, "BACKSPACE" }, 
	{ KEY_TAB, "TAB" }, 
	{ KEY_CLEAR	, "CLEAR" }, 
	{ KEY_RETURN, "RETURN" }, 
	{ KEY_PAUSE	, "PAUSE" }, 
	{ KEY_ESCAPE, "ESCAPE" }, 
	{ KEY_SPACE, "SPACE" }, 
	{ KEY_DELETE, "DELETE" }, 

	/* Numeric keypad */
	{ KEY_KP0, "KP0" }, 
	{ KEY_KP1, "KP1" }, 
	{ KEY_KP2, "KP2" }, 
	{ KEY_KP3, "KP3" }, 
	{ KEY_KP4, "KP4" }, 
	{ KEY_KP5, "KP5" }, 
	{ KEY_KP6, "KP6" }, 
	{ KEY_KP7, "KP7" }, 
	{ KEY_KP8, "KP8" }, 
	{ KEY_KP9, "KP9" }, 
	{ KEY_KP_PERIOD	, "KPDEL" }, 
	{ KEY_KP_DIVIDE	, "KP/" }, 
	{ KEY_KP_MULTIPLY, "KP*" }, 
	{ KEY_KP_MINUS, "KP-" }, 
	{ KEY_KP_PLUS, "KP+" }, 
	{ KEY_KP_ENTER, "KPENTER" }, 
	{ KEY_KP_EQUALS, "KP=" }, 

	/* Arrows + Home/End pad */
	{ KEY_UP, "UP" }, 
	{ KEY_DOWN, "DOWN" }, 
	{ KEY_RIGHT	, "RIGHT" }, 
	{ KEY_LEFT, "LEFT" }, 
	{ KEY_INSERT, "INSERT" }, 
	{ KEY_HOME, "HOME" }, 
	{ KEY_END, "END" }, 
	{ KEY_PAGEUP, "PAGEUP" }, 
	{ KEY_PAGEDOWN, "PAGEDOWN" }, 

	/* Function keys */
	{ KEY_F1, "F1" }, 
	{ KEY_F2, "F2" }, 
	{ KEY_F3, "F3" }, 
	{ KEY_F4, "F4" }, 
	{ KEY_F5, "F5" }, 
	{ KEY_F6, "F6" }, 
	{ KEY_F7, "F7" }, 
	{ KEY_F8, "F8" }, 
	{ KEY_F9, "F9" }, 
	{ KEY_F10, "F10" }, 
	{ KEY_F11, "F11" }, 
	{ KEY_F12, "F12" }, 
	{ KEY_F13, "F13" }, 
	{ KEY_F14, "F14" }, 
	{ KEY_F15, "F15" }, 

	/* Key state modifier keys */
	{ KEY_NUMLOCK, "NUMLOCK" },
	{ KEY_CAPSLOCK, "CAPSLOCK" },
	{ KEY_SCROLLOCK, "SCROLLOCK" },
	{ KEY_RSHIFT, "RIGHT_SHIFT" },
	{ KEY_LSHIFT, "LEFT_SHIFT" },
	{ KEY_RCTRL, "RIGHT_CTRL" },
	{ KEY_LCTRL, "LEFT_CTRL" },
	{ KEY_RALT, "RIGHT_ALT" },
	{ KEY_LALT, "LEFT_ALT" },
	{ KEY_RMETA, "X5" },
	{ KEY_LMETA, "X6" },
	{ KEY_LSUPER, "RIGHT_WINDOWS" },		/* Left "Windows" key */
	{ KEY_RSUPER, "LEFT_WINDOWS" },		/* Right "Windows" key */
	{ KEY_MODE, "ALT_GR" },		/* "Alt Gr" key */
	{ KEY_COMPOSE, "COMPOSE" },		/* Multi-key compose key */

	/* Miscellaneous function keys */
	{ KEY_HELP, "X0" },
	{ KEY_PRINT	, "X2" },
	{ KEY_SYSREQ, "X4" },
	{ KEY_BREAK, "PAUSE" },
	{ KEY_MENU, "X3" },
	{ KEY_POWER, "X1" },		/* Power Macintosh power key */
	{ KEY_EURO, "X2" },		/* Some european keyboards */

	// treat mouse buttons as keyboard too
	{ KEY_MOUSE1, "MOUSE1" }, 
	{ KEY_MOUSE2, "MOUSE2" }, 
	{ KEY_MOUSE3, "MOUSE3" }, 
	{ KEY_MOUSEWHEELUP, "MOUSEWHEELUP" }, 
	{ KEY_MOUSEWHEELDOWN, "MOUSEWHEELDOWN" },

};
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Input *input;

Input::Input()
{
	bind = new Binds();
	for (int i=0; i < NUM_KEYS; i++)
		keystate[i] = false;
}

Input::~Input()
{

}

void Input::KeyUp (int key)
{	
	keystate[key] = false;

	if (key == '`')
		con->Toggle();
	else if (con->visible)
		cedit->KeyUp(key);

	bind->Execute(key, true);// do binds

}

void Input::KeyDown (int key)
{
	keystate[key] = true;

	
	if ((con->visible) && (key != '`'))
		if (cedit->KeyDown(key))
			bind->Execute(key);	// do binds
	
	if (!con->visible)
		bind->Execute(key);
}

//make sure theres no white space
int Input::Code (char *str)
{
	int i, num = sizeof(keynames)/sizeof(KeyCode);

	for (i=0; i < num; i++)
	{
		if (stricmp(keynames[i].ident, str) == 0)
			return keynames[i].code;
	}
	if (strlen(str) == 1)
		return str[0];// toupper() / tolower()
	else
		return KEY_UNKNOWN;
}

char *Input::KeyString (int code)
{
	int i, num = sizeof(keynames)/sizeof(KeyCode);
	
	for (i=0; i < num; i++)
	{
		if (keynames[i].code == code)
			return keynames[i].ident;
	}

	return keynames[0].ident;
}