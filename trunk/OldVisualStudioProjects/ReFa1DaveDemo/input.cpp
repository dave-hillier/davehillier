// keyboard handler
// TODO
// allow key bindings
#include <stdio.h>
#include <windows.h>
extern HWND ghWnd;
#include "mathlib.h"
#include "input.h"
#include "glapp.h"
#include "timer.h"
#include "Console.h"
keycode_t keynames[] = {

{ VK_UNKNOWN, "UNKNOWN" }, 
{ VK_BACK, "BACKSPACE" }, 
{ VK_TAB, "TAB" }, 
{ VK_CLEAR	, "CLEAR" }, 
{ VK_RETURN, "RETURN" }, 
{ VK_PAUSE	, "PAUSE" }, 
{ VK_ESCAPE, "ESCAPE" }, 
{ VK_SPACE, "SPACE" }, 
{ VK_DELETE, "DELETE" }, 

/* Numeric keypad */
{ VK_NUMPAD0, "KP0" }, 
{ VK_NUMPAD1, "KP1" }, 
{ VK_NUMPAD2, "KP2" }, 
{ VK_NUMPAD3, "KP3" }, 
{ VK_NUMPAD4, "KP4" }, 
{ VK_NUMPAD5, "KP5" }, 
{ VK_NUMPAD6, "KP6" }, 
{ VK_NUMPAD7, "KP7" }, 
{ VK_NUMPAD8, "KP8" }, 
{ VK_NUMPAD9, "KP9" }, 
{ VK_DECIMAL	, "KPDEL" }, 
{ VK_DIVIDE	, "KP/" }, 
{ VK_MULTIPLY, "KP*" }, 
{ VK_SUBTRACT, "KP-" }, 
{ VK_ADD, "KP+" }, 
//{ VK_NUMPAD_ENTER, "KPENTER" }, 
//{ VK_NUMPAD_EQUALS, "KP=" }, 

/* Arrows + Home/End pad */
{ VK_UP, "UP" }, 
{ VK_DOWN, "DOWN" }, 
{ VK_RIGHT	, "RIGHT" }, 
{ VK_LEFT, "LEFT" }, 
{ VK_INSERT, "INSERT" }, 
{ VK_HOME, "HOME" }, 
{ VK_END, "END" }, 
{ VK_PRIOR, "PAGEUP" }, 
{ VK_NEXT, "PAGEDOWN" }, 

/* Function keys */
{ VK_F1, "F1" }, 
{ VK_F2, "F2" }, 
{ VK_F3, "F3" }, 
{ VK_F4, "F4" }, 
{ VK_F5, "F5" }, 
{ VK_F6, "F6" }, 
{ VK_F7, "F7" }, 
{ VK_F8, "F8" }, 
{ VK_F9, "F9" }, 
{ VK_F10, "F10" }, 
{ VK_F11, "F11" }, 
{ VK_F12, "F12" }, 
{ VK_F13, "F13" }, 
{ VK_F14, "F14" }, 
{ VK_F15, "F15" }, 

{ VK_NUMLOCK, "NUMLOCK" },
{ VK_CAPITAL, "CAPSLOCK" },
{ VK_SCROLL, "SCROLLOCK" },
{ VK_RSHIFT, "RIGHT_SHIFT" },
{ VK_LSHIFT, "LEFT_SHIFT" },
{ VK_LCONTROL, "RIGHT_CTRL" },
{ VK_LCONTROL, "LEFT_CTRL" },
//{ VK_RALT, "RIGHT_ALT" },
//{ VK_LALT, "LEFT_ALT" },

{ VK_LWIN, "RIGHT_WINDOWS" },		/* Left "Windows" VK */
{ VK_RWIN, "LEFT_WINDOWS" },		/* Right "Windows" VK */

/* Miscellaneous function VKs */
{ VK_SNAPSHOT	, "PRINT_SCREEN" },
{ VK_PAUSE, "PAUSE" },

// treat mouse buttons as keyboard too
{ VK_LBUTTON, "MOUSE1" }, 
{ VK_RBUTTON, "MOUSE2" }, 
{ VK_MBUTTON, "MOUSE3" }, 
{ VK_MWHEELUP, "MOUSEWHEELUP" }, 
{ VK_MWHEELDOWN, "MOUSEWHEELDOWN" },

};


Input input;

Input::Input()
{
	mouse_x = 0;
	mouse_y = 0;

	keyshiftlookup['1'] = '!';
	keyshiftlookup['2'] = '"';
	keyshiftlookup['3'] = '£';
	keyshiftlookup['4'] = '$';
	keyshiftlookup['5'] = '%';
	keyshiftlookup['6'] = '^';
	keyshiftlookup['7'] = '&';
	keyshiftlookup['8'] = '*';
	keyshiftlookup['9'] = '(';
	keyshiftlookup['0'] = ')';

	keyshiftlookup[189] = '_';
	keyshiftlookup[187] = '+';
	keyshiftlookup[222] = '~';
	keyshiftlookup[219] = '{';
	keyshiftlookup[221] = '}';
	keyshiftlookup[186] = ':';
	keyshiftlookup[192] = '@';
	keyshiftlookup[188] = '<';
	keyshiftlookup[190] = '>';
	keyshiftlookup[191] = '?';
	keyshiftlookup[' '] = ' ';

	keylookup[189] = '-';
	keylookup[187] = '=';
	keylookup[222] = '#';
	keylookup[219] = '[';
	keylookup[221] = ']';
	keylookup[186] = ';';
	keylookup[192] = '\'';
	keylookup[188] = ',';
	keylookup[190] = '.';
	keylookup[191] = '/';

	keylookup['1'] = '1';
	keylookup[' '] = ' ';
	keylookup['2'] = '2';
	keylookup['3'] = '3';
	keylookup['4'] = '4';
	keylookup['5'] = '5';
	keylookup['6'] = '6';
	keylookup['7'] = '7';
	keylookup['8'] = '8';
	keylookup['9'] = '9';
	keylookup['0'] = '0';
}

Input::~Input()
{

}

// key down
void Input::Down (int ch)
{
	keys[ch] = true;

	// check bindings/console typing etc
	if (ch == 223)
	{
		con->Toggle();
		return;
	}
	else if (ch == VK_ESCAPE)
	{
		PostMessage(ghWnd, WM_QUIT, 0, 0L);
		return;
	}
	
	if (con->visible)
	{

		if (ch == VK_RETURN)
		{
			// command parse
			con->Printf("\n");
		}
		else
		{
			short lshift = GetAsyncKeyState (VK_LSHIFT), 
				rshift = GetAsyncKeyState (VK_RSHIFT);
			short caps = GetKeyState (VK_CAPITAL);

			// Check caps lock state
			// get the 'high-order' bit
			// i guess it means most significant Byte

			if ((rshift & 0x8000) | (lshift & 0x8000))
			{
				if (isalpha(ch))
				{
					if (caps)
						ch = tolower(ch);
					else
						ch = toupper(ch);
				}
				else
				{
					ch = keyshiftlookup[ch];

				}
				
				// should really pass to a command buffer		
				con->Printf("%c", ch);
			}
			else
			{
				if (isalpha(ch))
				{
					if (caps)
						ch = toupper(ch);
					else
						ch = tolower(ch);
				}
				else
				{
					ch = keylookup[ch];
					
				}

				// should really pass to a command buffer		
				con->Printf("%c", (char)ch);
			}

		}
	}


}

// key up
void Input::Up (int ch)
{
	keys[ch] = false;
	
	// check (up)bindings
}

void Input::MouseUpdate()
{
	extern bool isFocus;
	if (!isFocus)
	{
		mouse_x = 0;
		mouse_y = 0;
		return;
	}
	POINT pt;
	int mx, my;
	static int old_mouse_x=0, old_mouse_y=0;


	if (!GetCursorPos (&pt))
		return;

	RECT r;
	GetWindowRect(ghWnd, &r);
	int midx = (r.left + r.right) / 2;
	int midy = (r.top + r.bottom) / 2;

	mx = pt.x - midx;
	my = pt.y - midy;

	if (true)// filter
	{
		mouse_x = (mx + old_mouse_x) * 0.5f;
		mouse_y = (my + old_mouse_y) * 0.5f;
	}
	else
	{
		mouse_x = (float)mx;
		mouse_y = (float)my;
	}

	old_mouse_x = mx;
	old_mouse_y = my;

/*	mouse_x *= scale;
	mouse_y *= scale;	*/// scale for sensitivity

/*	if (mlook)
	{
		camera.RotateX(mouse_y); // multiply for sep axis sennsitivity
		camera.RotateY(-mouse_x); // 
	}*/



	if (mx || my)
		SetCursorPos (midx, midy);

}


void Input::Update()
{
	MouseUpdate();


	if (keys['Z'])
		timer.frame_scale += 0.01f;
	if (keys['X'])
		timer.frame_scale -= 0.01f;

	if (timer.frame_scale < 0)
		timer.frame_scale = 2;
	if (timer.frame_scale > 1)
		timer.frame_scale = 1;

	// check state of left/right keys
}
