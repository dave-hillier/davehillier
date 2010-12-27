#include "project003.h"
/*
Keyboard & Other windows key messages, only temporary
*/
void KeyDown (int wParam)
{
	switch (wParam) 
	{ 

    case VK_F12:
		ScreenShot("saved.tga");
		break;
    case 91:
    case 'Q':
		ShutdownVid();
		PostQuitMessage(0);
		exit(0);
		break;
	case VK_LEFT: 
        view.pos [0]--;
        break; 
    case VK_RIGHT: 
        view.pos [0]++;
        break; 
    case VK_UP: 
        view.pos [2]--;
        break; 
    case VK_DOWN: 
        view.pos [2]++;
        break; 
    case VK_SHIFT: 
        view.pos [1]--;
        break; 
    case VK_CONTROL: 
        view.pos [1]++;
        break; 
	case 'Z':
		view.pos[0] = view.pos[1] = 0;
		view.pos[2] = 10;
		view.angles[0] = view.angles[1] = view.angles[2] = 0;
		break;
	case 'A':
		view.angles[0]--;
		break;
	case 'D':
		view.angles[0]++;
		break;		
	case 'W':
		view.angles[2]--;
		break;
 	case 'S':
		view.angles[2]++;
		break;
   } 	
}

void KeyUp (int wParam)
{

}

