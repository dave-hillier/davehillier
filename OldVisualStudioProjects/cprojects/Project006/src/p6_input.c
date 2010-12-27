#include <windows.h>
#include "p6_common.h"

extern vec3_t	viewpos;
extern vec3_t  viewangles;
extern int desiredtris;

void KeyDown (int wParam)
{
	switch (wParam) 
	{ 
	case VK_LEFT: 
        viewpos [0]-=10;
        break; 
    case VK_RIGHT: 
        viewpos [0]+=10;
        break; 
    case VK_UP: 
        viewpos [2]-=10;
        break; 
    case VK_DOWN: 
        viewpos [2]+=10;
        break; 
    case VK_SHIFT: 
        viewpos [1]-=10;
        break; 
    case VK_CONTROL: 
        viewpos [1]+=30;
        break; 
	case 'A':
		viewangles[0] -= 10;
		break;
	case 'D':
		viewangles[0] += 10;
		break;		
	case 'W':
		viewangles[2] -= 10;
		break;
 	case 'S':
		viewangles[2] +=10;
		break;
	case 'Z': 
        viewpos [0] = 512;
        viewpos [1] = 255;
        viewpos [2] = 512;
		break;
	case '0':
		if (desiredtris < 20000)
			desiredtris += 500;
		break;
	case '9':
		if (desiredtris > 0)
			desiredtris -= 500;
		break;

	}

}