#include <windows.h>
#include "common.h"

void KeyDown (int wParam)
{
	switch (wParam) 
	{ 

    case 'Q':
		PostQuitMessage(0);
		exit(0);
		break;
	case VK_LEFT: 
        view_pos [0]--;
        break; 
    case VK_RIGHT: 
        view_pos [0]++;
        break; 
    case VK_UP: 
        view_pos [2]--;
        break; 
    case VK_DOWN: 
        view_pos [2]++;
        break; 
    case VK_SHIFT: 
        view_pos [1]--;
        break; 
    case VK_CONTROL: 
        view_pos [1]++;
        break; 
	case 'Z':
		view_pos[0] = view_pos[1] = 0;
		view_pos[2] = 100;
		view_angles[0] = view_angles[1] = view_angles[2] = 0;
		break;
	case 'X':
		view_pos[0] = view_pos[1] = view_pos[2] = 0;
		view_angles[0] = view_angles[1] = view_angles[2] = 0;
		break;
	case 'A':
		view_angles[0]--;
		break;
	case 'D':
		view_angles[0]++;
		break;		
	case 'W':
		view_angles[2]--;
		break;
 	case 'S':
		view_angles[2]++;
		break;
   } 	
}