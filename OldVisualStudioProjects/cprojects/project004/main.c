#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include <math.h>
#include <string.h>
#include "common.h"

typedef enum {
	BUTTON1,
	BUTTON2,
	BUTTON3
} mouse_buttons_t;
mouse_buttons_t mouse_buttons = 0;		
int old_mx, mx, old_my, my;

// globals
HWND ghWnd;
HINSTANCE ghInstance;
HGLRC ghGLRC;
HDC ghDC;
int width, height/*, bpp*/;
float view_angles[3], view_pos[3];
// globals end



void AngleVectors (vec3_t angles, vec3_t forward, vec3_t right, vec3_t up)
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	
	angle = angles[YAW] * (M_PI*2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[PITCH] * (M_PI*2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[ROLL] * (M_PI*2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	forward[0] = cp*cy;
	forward[1] = cp*sy;
	forward[2] = -sp;
	right[0] = (-1*sr*sp*cy+-1*cr*-sy);
	right[1] = (-1*sr*sp*sy+-1*cr*cy);
	right[2] = -1*sr*cp;
	up[0] = (cr*sp*cy+-sr*-sy);
	up[1] = (cr*sp*sy+-sr*cy);
	up[2] = cr*cp;
}

long FAR PASCAL WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
    PAINTSTRUCT ps;

	switch(uMsg)
    {
	case WM_KEYDOWN: 
    case WM_SYSKEYDOWN:
		KeyDown(wParam);
		break;
	case WM_LBUTTONDOWN:
		mouse_buttons |= BUTTON1;
		return 0;
	case WM_RBUTTONDOWN:
		mouse_buttons |= BUTTON2;
		return 0;
	case WM_MBUTTONDOWN:
		mouse_buttons |= BUTTON3;
		return 0;

	case WM_LBUTTONUP:
		mouse_buttons &= ~BUTTON1;
		{
			vec3_t	forward, up, right;
			AngleVectors(view_angles, forward, right, up);
			view_pos[0] += up[0];
			view_pos[1] += up[1];
			view_pos[2] += up[2];
		}
		return 0;
	case WM_RBUTTONUP:
		{
			vec3_t	forward, up, right;
			AngleVectors(view_angles, forward, right, up);
			view_pos[0] -= up[0];
			view_pos[1] -= up[1];
			view_pos[2] -= up[2];
		}
		mouse_buttons &= ~BUTTON2;
		return 0;
	case WM_MBUTTONUP:
		mouse_buttons &= ~BUTTON3;
		{
			char str[32];
			sprintf(str, "%d %d", mx, my);
			MessageBox(NULL, str, "mouse 3", MB_OK);
		}
		return 0;
		return 0;

	case WM_MOUSEMOVE: 
		old_mx = mx;
		old_my = my;
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		view_angles[0] += (old_mx - mx);
		view_angles[2] += (old_my - my);

	/*	if ((old_mx - mx) || (old_my - my))
			SetCursorPos(width/2, height/2);*/

		return 0;

	case WM_ACTIVATE:
		break;

	case WM_CREATE:
		break;

	case WM_MOVE:
		break;

	case WM_SIZE:
		break;

	case WM_SETCURSOR:
	//	SetCursor(NULL);
		return TRUE;

	case WM_ERASEBKGND:
		return TRUE;

	case WM_PAINT:
		ghDC = BeginPaint(ghWnd, &ps);
		EndPaint(ghWnd, &ps);
		return TRUE;

	case WM_DESTROY:
		PostQuitMessage(0);
	break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam); 
}

bool bSetupPixelFormat(HDC hDC)
{
    static PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
	1,				// version number
	PFD_DRAW_TO_WINDOW 		// support window
	|  PFD_SUPPORT_OPENGL 	// support OpenGL
	|  PFD_DOUBLEBUFFER ,	// double buffered
	PFD_TYPE_RGBA,			// RGBA type
	24,					// 24-bit color depth
	0, 0, 0, 0, 0, 0,	// color bits ignored
	0,					// no alpha buffer
	0,					// shift bit ignored
	0,					// no accumulation buffer
	0, 0, 0, 0, 		// accum bits ignored
	32,					// 32-bit z-buffer	
	0,					// no stencil buffer
	0,					// no auxiliary buffer
	PFD_MAIN_PLANE,		// main layer
	0,					// reserved
	0, 0, 0				// layer masks ignored
    };
    int pixelformat;

    if ( (pixelformat = ChoosePixelFormat(ghDC, &pfd)) == 0 )
    {
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK);
        return false;
    }

    if (SetPixelFormat(ghDC, pixelformat, &pfd) == FALSE)
    {
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK);
        return false;
    }

    return true;
}
bool Init ()
{
//	MSG			msg; 
	WNDCLASS    wc;

	{
		wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; 
		wc.lpfnWndProc   = (WNDPROC)WindowProc; 
		wc.cbClsExtra    = 0; 
		wc.cbWndExtra    = 0; 
		wc.hInstance     = ghInstance; 
		wc.hIcon         = LoadIcon (ghInstance, NAME); 
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH) COLOR_GRAYTEXT;
		wc.lpszMenuName  = NULL; 
		wc.lpszClassName = NAME; 
	}

	if (!RegisterClass (&wc) ) 
	{
		MessageBox(NULL, "RegisterClass failed", "Error", MB_OK);
        return false;
	}
    ghWnd = CreateWindowEx ( 
		   0,//WS_EX_TOPMOST,
		   NAME, 
           TITLE,
		   WS_OVERLAPPED | WS_CAPTION,
		   //WS_POPUP | WS_VISIBLE,//fullscreen
		   // Dont know about these
           CW_USEDEFAULT,
           CW_USEDEFAULT,
           //0,
           //0,
           width,// Width 
		   height,// height
           NULL,
           NULL,
           (HINSTANCE)ghInstance,
           NULL);

    if (!ghWnd)
	{        
		MessageBox(NULL, "CreateWindowEx failed", "Error", MB_OK);
        return false; 
	}
 
    /* show and update main window */ 
    ShowWindow (ghWnd, SW_SHOW); 
 
    UpdateWindow (ghWnd);

	if ((ghDC = GetDC(ghWnd)) == NULL)
	{
        MessageBox(NULL, "GetDC failed", "Error", MB_OK);
		return false;
	}

	bSetupPixelFormat(ghDC);
 
	if ((ghGLRC = wglCreateContext( ghDC )) == NULL)
	{
        MessageBox(NULL, "wglCreateContext failed", "Error", MB_OK);
		return false;
	}
	if (!wglMakeCurrent(ghDC, ghGLRC))
	{
        MessageBox(NULL, "wglMakeCurrent failed", "Error", MB_OK);
		return false;
	}

	GLInit();

	return true;
}

bool fullscreen ()
{
	DEVMODE devMode;
	int i;
	bool stat;
	int bpp = 16;

	// Get all the modes and test em
	for (i=0;true;i++)
	{
		stat = EnumDisplaySettings (NULL, i, &devMode);

		if (
			( (int)devMode.dmPelsWidth == width) &&
			( (int)devMode.dmPelsHeight == height) &&
			( devMode.dmFields == (DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL)) &&
			( (int)devMode.dmBitsPerPel == bpp )
		   )
		{
			if (ChangeDisplaySettings (&devMode, CDS_TEST | CDS_FULLSCREEN) 
				== DISP_CHANGE_SUCCESSFUL)
			break;
		}
		if (!stat)
		{
			MessageBox(NULL, "fullscreen failed", "Error", MB_OK);
			return false;
		}
	}

	if (ChangeDisplaySettings(&devMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		MessageBox(NULL, "fullscreen failed", "Error", MB_OK);
		return false;
	}

	// This doesnt work for my 3dfx v3 card, it may work on the v banshee
	ShowCursor(0);
	return true;
}


bool aFullscreen ()
{
	DEVMODE devMode;
	int bpp = 16, mode = 0;

	ZeroMemory(&devMode, sizeof(DEVMODE));

	devMode.dmSize       = sizeof(DEVMODE);
	devMode.dmPelsWidth	= width;				
	devMode.dmPelsHeight = height;				
	devMode.dmBitsPerPel = bpp;					
	devMode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

	if (ChangeDisplaySettings(&devMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		MessageBox(NULL, "Ln 335: Fullscreen failed", "Error", MB_OK);
		return false;
	}

	ShowCursor(0);
	return true;
}
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG        msg; 

	ghInstance = hInstance;
	width = 300;/*100;*/ 
	height = 300;/*100;*/

//	aFullscreen();
	if (Init ())
	{
	
		while (true) 
		{ 
			while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE) 
			{ 
				if (GetMessage(&msg, NULL, 0, 0) ) 
				{ 
					TranslateMessage(&msg); 
					DispatchMessage(&msg); 
				} else { 
					return TRUE; 
				} 	
			} 
			Render();		
		}
	}
	return TRUE; 
}