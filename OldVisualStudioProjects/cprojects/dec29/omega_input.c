#include "omega_common.h"

void KeyDown (int wParam)
{
	switch (wParam) 
	{ 
	case 'Q':
	case 'q':
	case 27:
		VidShutdown();
		SaveConfig();
		PostQuitMessage(0);
		exit(0);
		break;
	case VK_F11:
		VidGoFullScreen();// TODO: Fix Screen changing
		current_vidmode.fullscreen = true;
		break;
	case VK_F12:
		VidGoWindowed();// TODO: Fix Screen changing
		current_vidmode.fullscreen = false;
		break;
	case VK_F10:

		{
			char str[40];
			sprintf(str, "fps: %f", fps);
			MessageBox(NULL, str, "Time", MB_OK);
		}
	case '1':
		{
			vec3_t pos;
			VectorSet(pos, -80, 0, 0);

			pManager.systems = pAddSystem(0, pos, 10, 1000, pManager.systems);
		}
		break;
	case '2':
		{
			vec3_t pos;
			VectorSet(pos, 80, 0, 0);

			pManager.systems = pAddSystem(1, pos, 10, 1000, pManager.systems);
		}
		break;
	}
}
// TODO: mouse capture