#include "omega_common.h"
#include "omega_gl.h"
mdh *ModelPointer (char *name);
entity *GetEntity ();

extern bool showfps;
extern int triused;
void KeyDown (int wParam)
{
	if (ConInput(wParam))
		return;
	
	switch (wParam) 
	{ 
	case 27:// esc
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
		
		return;
	case VK_F9:
		ConsoleToggle();
		return;
	case VK_F8:
		if (showfps) 
			showfps = false;
		else
			showfps = true;
		return;
	case VK_F7:
		ScreenShot();
		return;
	case VK_F6:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		return;
	case VK_F5:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		return;
	case '1':
		{
			vec3_t pos;
			VectorSet(pos, 512, 300, 512);
			ConPrintf("Particles 1 activated\n");

			pManager.systems = pAddSystem(0, pos, 10, 100, pManager.systems);
		}
		break;
	case '2':
		{
			vec3_t pos;
			VectorSet(pos, 512, 300, 512);
			ConPrintf("Particles 2 activated\n");

			pManager.systems = pAddSystem(1, pos, 10, 100, pManager.systems);
		}
		break;
	case '3':
		{
			vec3_t pos;
			VectorSet(pos, 512, 300, 512);
			ConPrintf("Particles 3 activated\n");

			pManager.systems = pAddSystem(0, pos, 1000, 10000, pManager.systems);
		}
		break;
	case '4':
		{
			vec3_t pos;
			VectorSet(pos, 512, 300, 512);
			ConPrintf("Particles 4 activated\n");

			pManager.systems = pAddSystem(1, pos, 100, 100, pManager.systems);
		}
		break;
	case '5':
		{
			entity *e = GetEntity();
			vec3_t f;
			float m[16];
			
			glGetFloatv(GL_MODELVIEW_MATRIX, m);
			VectorSet(f, -m[2], -m[6], -m[10]);
			VectorScale(f, 10, f);
			VectorCopy(f, e->velocity);
			VectorCopy(viewer.pos, e->position);
						
			e->model = ModelPointer("rocket.dh4");
		}
		break;
	case '6':
		{
			entity *e = GetEntity();
			vec3_t f;
			float m[16];
			
			glGetFloatv(GL_MODELVIEW_MATRIX, m);
			VectorSet(f, -m[2], -m[6], -m[10]);
			VectorScale(f, 10, f);
			VectorCopy(f, e->velocity);
			VectorCopy(viewer.pos, e->position);
			VectorSet(e->maxs, 128, 128, 128);
			VectorSet(e->mins, -128, -128, -128);

			e->model = ModelPointer("ball.dh4");
		}
		break;
	case VK_LEFT: 
        viewer.pos [0]-=10;
        break; 
    case VK_RIGHT: 
        viewer.pos [0]+=10;
        break; 
    case VK_UP: 
        viewer.pos [2]-=10;
        break; 
    case VK_DOWN: 
        viewer.pos [2]+=10;
        break; 
    case VK_SHIFT: 
        viewer.pos [1]-=10;
        break; 
    case VK_CONTROL: 
        viewer.pos [1]+=30;
        break; 
	case 'Z':
		viewer.pos[0] = 512;
		viewer.pos[1] = 255;
		viewer.pos[2] = 512;
		viewer.angles[0] = viewer.angles[1] = viewer.angles[2] = 0;
		break;
	case 'X':
		viewer.pos[0] = viewer.pos[1] = viewer.pos[2] = 0;
		viewer.angles[0] = viewer.angles[1] = viewer.angles[2] = 0;
		break;
	case 'A':
		viewer.angles[0] -= 10;
		break;
	case 'D':
		viewer.angles[0] += 10;
		break;		
	case 'W':
		viewer.angles[2] -= 10;
		break;
 	case 'S':
		viewer.angles[2] +=10;
		break;
	case 'B':
		{
			vec3_t f, r, u;
			float m[16];
			
			glGetFloatv(GL_MODELVIEW_MATRIX, m);

			VectorSet(r, m[0], m[4], m[8]);
			VectorSet(u, m[1], m[5], m[9]);
			VectorSet(f, -m[2], -m[6], -m[10]);

			VectorScale(u, 10, u);
			VectorScale(r, 10, r);
			VectorScale(f, 10, f);
			VectorAdd(viewer.pos, f, viewer.pos);
			break;
		}
	case 'V':
		{
			vec3_t f, r, u;
			float m[16];
			
			glGetFloatv(GL_MODELVIEW_MATRIX, m);

			VectorSet(r, m[0], m[4], m[8]);
			VectorSet(u, m[1], m[5], m[9]);
			VectorSet(f, -m[2], -m[6], -m[10]);

			VectorScale(u, 10, u);
			VectorScale(r, 10, r);
			VectorScale(f, 10, f);
			VectorAdd(viewer.pos, r, viewer.pos);
			break;
		}
	case 'C':
		{
			vec3_t f, r, u;
			float m[16];
			
			glGetFloatv(GL_MODELVIEW_MATRIX, m);

			VectorSet(r, m[0], m[4], m[8]);
			VectorSet(u, m[1], m[5], m[9]);
			VectorSet(f, -m[2], -m[6], -m[10]);

			VectorScale(u, 10, u);
			VectorScale(r, 10, r);
			VectorScale(f, 10, f);
			VectorAdd(viewer.pos, u, viewer.pos);
			break;
		}
	default:
		break;
	}
}
// TODO: mouse capture