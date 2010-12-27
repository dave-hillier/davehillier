#include <windows.h>
#include "common.h"


void KeyDown (int wParam)
{
	switch (wParam) 
	{ 
	case 'Q':
		VidShutdown();
		PostQuitMessage(0);
		exit(0);
		break;
	}
}
